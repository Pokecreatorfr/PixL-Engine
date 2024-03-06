#include <CoordCalculator.hpp>


CoordCalculator::CoordCalculator(){
    Camera = Camera::GetInstance();
}

CoordCalculator* CoordCalculator::instance_ = nullptr;

CoordCalculator* CoordCalculator::GetInstance(){
    if (instance_ == nullptr){
        instance_ = new CoordCalculator();
    }
    return instance_;
}

CoordCalculator::~CoordCalculator(){
}

int CoordCalculator::add_coord_to_adjust(int* coord, int type, float size){
    int_to_adjust* new_int_to_adjust = new int_to_adjust;
    new_int_to_adjust->coord = coord;
    new_int_to_adjust->type = type;
    new_int_to_adjust->size = size;
    if (int_to_adjusts.size() == 0){
        new_int_to_adjust->uid = 0;
    }
    else
    {
        int x = 0;
        bool found = false;
        while (!found){
            found = true;
            for (int i = 0; i < int_to_adjusts.size(); i++){
                if (int_to_adjusts[i]->uid == x){
                    found = false;
                    x++;
                    break;
                }
            }
        }
        new_int_to_adjust->uid = x;
    }
    int_to_adjusts.push_back(new_int_to_adjust);
    return new_int_to_adjust->uid;
}

void CoordCalculator::remove_coord_to_adjust(int uid){
    for (int i = 0; i < int_to_adjusts.size(); i++){
        if (int_to_adjusts[i]->uid == uid){
            delete int_to_adjusts[i];
            int_to_adjusts.erase(int_to_adjusts.begin() + i);
            break;
        }
    }
}

void CoordCalculator::adjust_coords(){
    for (int i = 0; i < int_to_adjusts.size(); i++){
        if (int_to_adjusts[i]->type == WIDTH){
            *int_to_adjusts[i]->coord = int_to_adjusts[i]->size * Camera->GetSize()->x; 
        }
        else if (int_to_adjusts[i]->type == HEIGHT){
            *int_to_adjusts[i]->coord = int_to_adjusts[i]->size * Camera->GetSize()->y;
        }
    }
}