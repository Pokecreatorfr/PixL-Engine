#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

enum class CameraType
{
    ORTHOGRAPHIC,
    PERSPECTIVE
};

class Camera
{
    Camera();
    static Camera *_instance;

public:
    static int Init();
    static int Quit();
    static int Update();

    static int SetType(CameraType t);
    static int SetAspect(float aspect);
    static int SetNearFar(float n, float f);
    static int SetFovDeg(float fovDeg);
    static int SetOrthoHalfHeight(float halfHeight);

    static int SetPosition(const glm::vec3 &p);
    static int GetPosition(glm::vec3 &out_pos);

    static int SetYawPitchDeg(float yawDeg, float pitchDeg);
    static int AddYawPitchDeg(float dyaw, float dpitch);

    static int Move(float forward, float right, float up);

    static int Forward(glm::vec3 &out_forward);
    static int Right(glm::vec3 &out_right);
    static int Up(glm::vec3 &out_up);

    static int GetViewMatrix(glm::mat4 &out_view);
    static int GetProjectionMatrix(glm::mat4 &out_proj);

private:
    static void RebuildView();
    static void RebuildProj();

private:
    CameraType _type = CameraType::PERSPECTIVE;

    float _fovDeg = 70.0f;
    float _aspect = 16.0f / 9.0f;
    float _near = 0.1f;
    float _far = 1000.0f;
    float _orthoHalfHeight = 1.0f;

    glm::vec3 _position = glm::vec3(0.0f, 0.0f, 3.0f);
    float _yawDeg = -90.0f;
    float _pitchDeg = 0.0f;
    glm::quat _orientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

    glm::vec3 _forward = glm::vec3(0, 0, -1);
    glm::vec3 _right = glm::vec3(1, 0, 0);
    glm::vec3 _up = glm::vec3(0, 1, 0);

    glm::mat4 _view = glm::mat4(1.0f);
    glm::mat4 _proj = glm::mat4(1.0f);

    bool _dirtyView = true;
    bool _dirtyProj = true;
};
