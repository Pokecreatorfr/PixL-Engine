#include <game/camera.hpp>

Camera *Camera::_instance = nullptr;

Camera::Camera() = default;

int Camera::Init()
{
    if (_instance)
        return 0;
    _instance = new Camera();
    _instance->_dirtyView = true;
    _instance->_dirtyProj = true;
    glm::mat4 dummy;
    (void)GetViewMatrix(dummy);
    (void)GetProjectionMatrix(dummy);
    return 0;
}

int Camera::Quit()
{
    if (!_instance)
        return 0;
    delete _instance;
    _instance = nullptr;
    return 0;
}

int Camera::Update()
{
    if (!_instance)
        return -1;

    if (_instance->_dirtyView)
        RebuildView();
    if (_instance->_dirtyProj)
        RebuildProj();

    return 0;
}

void Camera::RebuildView()
{
    if (!_instance)
        return;

    glm::vec3 f;
    f.x = cos(glm::radians(_instance->_yawDeg)) * cos(glm::radians(_instance->_pitchDeg));
    f.y = sin(glm::radians(_instance->_pitchDeg));
    f.z = sin(glm::radians(_instance->_yawDeg)) * cos(glm::radians(_instance->_pitchDeg));
    _instance->_forward = glm::normalize(f);

    const glm::vec3 worldUp(0.0f, 1.0f, 0.0f);
    _instance->_right = glm::normalize(glm::cross(_instance->_forward, worldUp));
    _instance->_up = glm::normalize(glm::cross(_instance->_right, _instance->_forward));

    _instance->_view = glm::lookAt(_instance->_position, _instance->_position + _instance->_forward, _instance->_up);
    _instance->_dirtyView = false;
}

void Camera::RebuildProj()
{
    if (!_instance)
        return;

    if (_instance->_type == CameraType::PERSPECTIVE)
    {
        _instance->_proj = glm::perspective(glm::radians(_instance->_fovDeg), _instance->_aspect, _instance->_near, _instance->_far);
    }
    else
    {
        float halfH = _instance->_orthoHalfHeight;
        float halfW = halfH * _instance->_aspect;
        _instance->_proj = glm::ortho(-halfW, halfW, -halfH, halfH, _instance->_near, _instance->_far);
    }
    _instance->_dirtyProj = false;
}

int Camera::SetType(CameraType t)
{
    if (!_instance)
        return -1;
    _instance->_type = t;
    _instance->_dirtyProj = true;
    return 0;
}

int Camera::SetAspect(float aspect)
{
    if (!_instance)
        return -1;
    _instance->_aspect = aspect;
    _instance->_dirtyProj = true;
    return 0;
}

int Camera::SetNearFar(float n, float f)
{
    if (!_instance)
        return -1;
    _instance->_near = n;
    _instance->_far = f;
    _instance->_dirtyProj = true;
    return 0;
}

int Camera::SetFovDeg(float fovDeg)
{
    if (!_instance)
        return -1;
    _instance->_fovDeg = fovDeg;
    _instance->_dirtyProj = true;
    return 0;
}

int Camera::SetOrthoHalfHeight(float halfHeight)
{
    if (!_instance)
        return -1;
    _instance->_orthoHalfHeight = halfHeight;
    _instance->_dirtyProj = true;
    return 0;
}

int Camera::SetPosition(const glm::vec3 &p)
{
    if (!_instance)
        return -1;
    _instance->_position = p;
    _instance->_dirtyView = true;
    return 0;
}

int Camera::GetPosition(glm::vec3 &out_pos)
{
    if (!_instance)
        return -1;
    out_pos = _instance->_position;
    return 0;
}

int Camera::SetYawPitchDeg(float yawDeg, float pitchDeg)
{
    if (!_instance)
        return -1;
    _instance->_yawDeg = yawDeg;
    _instance->_pitchDeg = glm::clamp(pitchDeg, -89.0f, 89.0f);
    _instance->_dirtyView = true;
    return 0;
}

int Camera::AddYawPitchDeg(float dyaw, float dpitch)
{
    if (!_instance)
        return -1;
    _instance->_yawDeg += dyaw;
    _instance->_pitchDeg = glm::clamp(_instance->_pitchDeg + dpitch, -89.0f, 89.0f);
    _instance->_dirtyView = true;
    return 0;
}

int Camera::Move(float forward, float right, float up)
{
    if (!_instance)
        return -1;
    if (_instance->_dirtyView)
        RebuildView();

    _instance->_position += _instance->_forward * forward;
    _instance->_position += _instance->_right * right;
    _instance->_position += _instance->_up * up;
    _instance->_dirtyView = true;
    return 0;
}

int Camera::Forward(glm::vec3 &out_forward)
{
    if (!_instance)
        return -1;
    if (_instance->_dirtyView)
        RebuildView();
    out_forward = _instance->_forward;
    return 0;
}

int Camera::Right(glm::vec3 &out_right)
{
    if (!_instance)
        return -1;
    if (_instance->_dirtyView)
        RebuildView();
    out_right = _instance->_right;
    return 0;
}

int Camera::Up(glm::vec3 &out_up)
{
    if (!_instance)
        return -1;
    if (_instance->_dirtyView)
        RebuildView();
    out_up = _instance->_up;
    return 0;
}

int Camera::GetViewMatrix(glm::mat4 &out_view)
{
    if (!_instance)
        return -1;
    if (_instance->_dirtyView)
        RebuildView();
    out_view = _instance->_view;
    return 0;
}

int Camera::GetProjectionMatrix(glm::mat4 &out_proj)
{
    if (!_instance)
        return -1;
    if (_instance->_dirtyProj)
        RebuildProj();
    out_proj = _instance->_proj;
    return 0;
}
