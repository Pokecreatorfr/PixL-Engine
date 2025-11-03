#pragma once
#include <cassert>
#include <cstdint>
#include <memory>
#include <type_traits>
#include <typeindex>
#include <unordered_map>
#include <utility>
#include <vector>

// Entity Component System

// Pas de logs , doit etre rapide

namespace pixl::core
{
    using EntityID = uint32_t;

    struct Entity
    {
        EntityID id{0}; // 8 bits gen | 24 bits id ( generation permet d'evtiter d'appeler depuis un identifiant mort)
        static constexpr uint32_t INDEX_BITS = 24;
        static constexpr uint32_t GEN_BITS = 8;
        static constexpr uint32_t INDEX_MASK = (1u << INDEX_BITS) - 1u;
        static constexpr uint32_t GEN_MASK = (1u << GEN_BITS) - 1u;

        uint32_t index() const { return id & INDEX_MASK; }
        uint32_t generation() const { return (id >> INDEX_BITS) & GEN_MASK; }
        explicit operator bool() const { return id != 0; }

        friend bool operator==(Entity a, Entity b) { return a.id == b.id; }
        friend bool operator!=(Entity a, Entity b) { return a.id != b.id; }
    };

    static inline Entity make_entity(uint32_t index, uint32_t gen)
    {
        return Entity{(EntityID)((index & Entity::INDEX_MASK) |
                                 ((gen & Entity::GEN_MASK) << Entity::INDEX_BITS))};
    }

    // set a crée une interface pour le stockage des composants
    struct IStorage
    {
        virtual ~IStorage() = default;
        virtual void on_entity_destroy(uint32_t idx) = 0;
        virtual bool contains(uint32_t idx) const = 0;
    };

    // permet de stocker les composants peu importe le type
    template <class T>
    class Storage : public IStorage
    {
    public:
        template <class... Args>
        T &emplace(uint32_t eidx, Args &&...args)
        {
            ensure_dense(eidx);
            if (contains(eidx))
            { // replace
                T &c = packed_[dense_[eidx]];
                c = T{std::forward<Args>(args)...};
                return c;
            }
            dense_[eidx] = (uint32_t)packed_.size();
            owners_.push_back(eidx);
            packed_.emplace_back(std::forward<Args>(args)...);
            return packed_.back();
        }

        void erase(uint32_t eidx)
        {
            if (!contains(eidx))
                return;
            uint32_t di = dense_[eidx];
            uint32_t last = (uint32_t)packed_.size() - 1;
            if (di != last)
            {
                packed_[di] = std::move(packed_[last]);
                uint32_t movedOwner = owners_[last];
                owners_[di] = movedOwner;
                dense_[movedOwner] = di;
            }
            packed_.pop_back();
            owners_.pop_back();
            dense_[eidx] = NONE;
        }

        T *get(uint32_t eidx)
        {
            if (!contains(eidx))
                return nullptr;
            return &packed_[dense_[eidx]];
        }
        const T *get(uint32_t eidx) const
        {
            if (!contains(eidx))
                return nullptr;
            return &packed_[dense_[eidx]];
        }

        bool contains(uint32_t eidx) const override
        {
            return eidx < dense_.size() && dense_[eidx] != NONE;
        }

        void on_entity_destroy(uint32_t eidx) override { erase(eidx); }

        const std::vector<uint32_t> &owners() const { return owners_; }
        std::vector<T> &data() { return packed_; }
        const std::vector<T> &data() const { return packed_; }

    private:
        static constexpr uint32_t NONE = 0xFFFFFFFFu;
        void ensure_dense(uint32_t i)
        {
            if (i >= dense_.size())
                dense_.resize(i + 1, NONE);
        }

        std::vector<uint32_t> owners_;
        std::vector<T> packed_;
        std::vector<uint32_t> dense_;
    };

    class ECS; // forward declaration so ECSCommandBuffer can refer to ECS before it's defined

    struct ECSCommandBuffer
    {
    public:
        struct AddBase
        {
            virtual ~AddBase() = default;
            virtual void apply(ECS &ecs) = 0;
        };
        template <class T>
        struct AddT : AddBase
        {
            Entity e;
            T value;
            void apply(ECS &ecs) override;
        };
        struct RemoveBase
        {
            virtual ~RemoveBase() = default;
            virtual void apply(ECS &ecs) = 0;
        };
        template <class T>
        struct RemoveT : RemoveBase
        {
            Entity e;
            void apply(ECS &ecs) override;
        };

        struct Destroy
        {
            Entity e;
        };

        std::vector<std::unique_ptr<AddBase>> adds;
        std::vector<std::unique_ptr<RemoveBase>> removes;
        std::vector<Destroy> destroys;

        template <class T>
        void add(Entity e, const T &v) { adds.emplace_back(std::make_unique<AddT<T>>(AddT<T>{e, v})); }
        template <class T>
        void remove(Entity e) { removes.emplace_back(std::make_unique<RemoveT<T>>(RemoveT<T>{e})); }
        void destroy(Entity e) { destroys.push_back({e}); }

        void clear()
        {
            adds.clear();
            removes.clear();
            destroys.clear();
        }
    };

    // Pas d'Init ni de Quit , c'est RAII
    class ECS
    {
    public:
        ECS() = default;

        Entity create()
        {
            uint32_t idx;
            if (!free_.empty())
            {
                idx = free_.back();
                free_.pop_back();
            }
            else
            {
                idx = (uint32_t)gens_.size();
                // hard cap at 2^24 indices
                assert(idx < (1u << Entity::INDEX_BITS) && "ECS: too many entities (exceeded 24-bit index)");
                gens_.push_back(0);
            }
            return make_entity(idx, gens_[idx]);
        }

        bool valid(Entity e) const
        {
            uint32_t idx = e.index();
            return e && idx < gens_.size() && gens_[idx] == e.generation();
        }

        void destroy(Entity e)
        {
            if (!valid(e))
                return;
            uint32_t idx = e.index();
            for (auto &[ti, s] : stores_)
                s->on_entity_destroy(idx);
            gens_[idx] = (uint8_t)((gens_[idx] + 1) & Entity::GEN_MASK);
            free_.push_back(idx);
        }

        template <class T, class... Args>
        T &add(Entity e, Args &&...args)
        {
            assert(valid(e) && "ECS.add: invalid entity");
            return storage<T>().emplace(e.index(), std::forward<Args>(args)...);
        }

        template <class T>
        bool has(Entity e) const
        {
            if (!valid(e))
                return false;
            auto it = stores_.find(std::type_index(typeid(T)));
            return it != stores_.end() && it->second->contains(e.index());
        }

        template <class T>
        T *get(Entity e)
        {
            if (!valid(e))
                return nullptr;
            return storage<T>().get(e.index());
        }
        template <class T>
        const T *get(Entity e) const
        {
            if (!valid(e))
                return nullptr;
            return storage<T>().get(e.index());
        }

        template <class T>
        void remove(Entity e)
        {
            if (!valid(e))
                return;
            storage<T>().erase(e.index());
        }

        template <class... Ts>
        struct View
        {
            struct It
            {
                const ECS *ecs{};
                const std::vector<uint32_t> *owners{};
                size_t i{};
                void advance()
                {
                    while (i < owners->size())
                    {
                        uint32_t idx = (*owners)[i];
                        Entity e = make_entity(idx, ecs->gens_[idx]);
                        if (ecs->valid(e) && (ecs->has<Ts>(e) && ...))
                            break;
                        ++i;
                    }
                }
                bool operator!=(const It &o) const { return i != o.i; }
                void operator++()
                {
                    ++i;
                    advance();
                }
                auto operator*() const
                {
                    uint32_t idx = (*owners)[i];
                    Entity e = make_entity(idx, ecs->gens_[idx]);
                    return std::tuple<Entity, Ts &...>(e, *ecs->template get<Ts>(e)...);
                }
            };

            const ECS *ecs{};
            const std::vector<uint32_t> *owners{};

            It begin() const
            {
                It it{ecs, owners, 0};
                it.advance();
                return it;
            }
            It end() const { return It{ecs, owners, owners->size()}; }
        };

        template <class... Ts>
        View<Ts...> view() const
        {
            const std::vector<uint32_t> *owners = nullptr;
            size_t minSize = SIZE_MAX;
            auto pick = [&](auto *s)
            {
                size_t sz = s->owners().size();
                if (sz < minSize)
                {
                    minSize = sz;
                    owners = &s->owners();
                }
            };
            (pick(&storage<Ts>()), ...);
            return View<Ts...>{this, owners ? owners : &storage<std::tuple_element_t<0, std::tuple<Ts...>>>().owners()};
        }

    private:
        template <class T>
        Storage<T> &storage() const
        {
            auto key = std::type_index(typeid(T));
            auto it = stores_.find(key);
            if (it == stores_.end())
            {
                auto *self = const_cast<ECS *>(this);
                auto *s = new Storage<T>();
                self->stores_.emplace(key, std::unique_ptr<IStorage>(s));
                return *s;
            }
            return *static_cast<Storage<T> *>(it->second.get());
        }

        std::vector<uint8_t> gens_;
        std::vector<uint32_t> free_;
        mutable std::unordered_map<std::type_index, std::unique_ptr<IStorage>> stores_;
    };

    inline void Apply(ECS &ecs, ECSCommandBuffer &cb)
    {
        for (auto &r : cb.removes)
            r->apply(ecs);
        for (auto &a : cb.adds)
            a->apply(ecs);
        for (auto &d : cb.destroys)
            ecs.destroy(d.e);
        cb.clear();
    }

    template <class T>
    inline void ECSCommandBuffer::AddT<T>::apply(ECS &ecs)
    {
        if (ecs.valid(e))
            ecs.add<T>(e, value);
    }

    template <class T>
    inline void ECSCommandBuffer::RemoveT<T>::apply(ECS &ecs)
    {
        if (ecs.valid(e))
            ecs.remove<T>(e);
    }
}
