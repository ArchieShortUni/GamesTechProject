#include "pch.h"
#include "camera.h"
#include "glm/gtc/matrix_transform.hpp"
#include "engine/core/input.h"
#include "engine/key_codes.h"


//================== Orthographic Camera [2D] =================================

engine::orthographic_camera::orthographic_camera(float left, float right, float bottom, float top)
    : m_projection_mat(glm::ortho(left, right, bottom, top, -1.f, 1.f))
{
    m_view_projection_mat = m_projection_mat * m_view_mat;
}

void engine::orthographic_camera::move(e_direction direction, timestep ts)
{
    float speed = s_movement_speed;

    if(direction == up)
        m_position.y += speed * ts;
    else if(direction == down)
        m_position.y -= speed * ts;

    if(direction == left)
        m_position.x -= speed * ts;
    else if(direction == right)
        m_position.x += speed * ts;

    update_view_matrix();
}

void engine::orthographic_camera::rotate(e_rotation rotation, e_axis, timestep ts)
{
    float speed = s_rotation_speed;

    if(rotation == clock_wise)
    {
        m_rotation -= speed * ts;
    }
    else if(rotation == anticlock_wise)
    {
        m_rotation += speed * ts;
    }

    update_view_matrix();
}

void engine::orthographic_camera::update_view_matrix()
{
    glm::mat4 transform(1);
    glm::rotate(transform, glm::radians(m_rotation), glm::vec3(0,0,1));
    transform = glm::translate(transform, m_position);

    // inverting the transform matrix 
    m_view_mat = glm::inverse(transform);
    m_view_projection_mat = m_projection_mat * m_view_mat;
}

//================== Perspective Camera [3D] ==================================

engine::perspective_camera::perspective_camera(
    float width, float height,
    float fov /*= 45.f*/,
    float near_z /*= 0.1f*/, float far_z /*= 100.f*/)
    : m_projection_mat(glm::perspective(glm::radians(fov), width / height, near_z, far_z)),
    m_aspect_ratio(width / height),
    m_fov(fov),
    m_near_plane(near_z),
    m_far_plane(far_z),
    m_width(width),
    m_height(height)
{ 
    //m_position = glm::vec3(0.0f, 1.0f, 3.0f);
    m_position = glm::vec3(0.0f, 2.0f, 20.0f);
    m_front_vector = glm::vec3(0.0f, 0.0f, -1.0f);
    m_up_vector = glm::vec3(0.0f, 1.0f,  0.0f);
    m_view_mat = glm::lookAt(m_position, m_position + m_front_vector, m_up_vector);

    glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    //m_front_vector = glm::normalize(m_position - cameraTarget);

    //glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f); 
    //m_righ_vector = glm::normalize(glm::cross(up, m_front_vector));

    //m_up_vector = glm::cross(m_front_vector, m_righ_vector);
    // note that we're translating the scene in the reverse direction of where we want to move 
    //m_view_mat = glm::translate(m_view_mat, start_position);
    m_view_projection_mat = m_projection_mat * m_view_mat; 
    LOG_CORE_TRACE("3d cam position: [{},{},{}]", m_position.x, m_position.y, m_position.z); 
    LOG_CORE_TRACE("3d cam rotation: [{},{},{}]", m_rotation_angle.x, m_rotation_angle.y, m_rotation_angle.z); 
}

void engine::perspective_camera::update_rail(const timestep& ts)
{
    m_position.z -= s_movement_speed * ts;
}

void engine::perspective_camera::on_update(const timestep& timestep)
{
    auto [mouse_delta_x, mouse_delta_y] = input::mouse_position();
    process_mouse(mouse_delta_x, mouse_delta_y);


    update_camera_vectors();


    if (input::key_pressed(engine::key_codes::KEY_A)) // left 

        move(e_direction::left, timestep);
    else if (input::key_pressed(engine::key_codes::KEY_D)) // right 
        move(e_direction::right, timestep);

    
    if (input::key_pressed(engine::key_codes::KEY_S)) // down 
        move(e_direction::backward, timestep);
    else if (engine::input::key_pressed(engine::key_codes::KEY_W)) // up 
        move(e_direction::forward, timestep);

    //HANDLE FPS STUFF HERE
    if (m_position.y > standing_height) { m_position.y = standing_height; }
    else if (m_position.y < crouching_height) { m_position.y = crouching_height; }


    if (!crouched) {
        if (m_position.y < standing_height) {
            m_position.y += crouch_step * timestep; 
        }
    }
    else {
        if (m_position.y > crouching_height) {
            m_position.y -= crouch_step * timestep; 
        }
    }


    if (!sprinting) {
        if (m_fov > min_fov) {
            m_fov -= (fov_step * (non_linear_multiplier* non_linear_multiplier)) * timestep;
            update_fov(m_fov);
            non_linear_multiplier += .1f;
        }
        else { non_linear_multiplier = 1.f; }

        if (sprint_speed > min_sprint_speed) {
            sprint_speed -= (sprint_step * (sprint_mulitplayer* sprint_mulitplayer)) * timestep;
            sprint_mulitplayer += .1f;
        }
        else { sprint_mulitplayer = 1.f; }

    }
    else {
        if (m_fov < max_fov) {
            m_fov += (fov_step* (non_linear_multiplier * non_linear_multiplier)) * timestep;
            update_fov(m_fov);
            non_linear_multiplier += .2f;
        }
        else { non_linear_multiplier = 1.f; }

        if (sprint_speed < max_sprint_speed) {
            sprint_speed += (sprint_step * (sprint_mulitplayer * sprint_mulitplayer)) * timestep;
            sprint_mulitplayer += .1f;
        }
        else { sprint_mulitplayer = 1.f; }
    }





  //  m_rotation_angle += glm::vec3(1.0f, 0.0f, 0.0f);
    
    rotate(clock_wise, y, timestep);


    //float delta = input::mouse_scroll(); 
    //process_mouse_scroll(delta); 
}

const glm::mat4& engine::perspective_camera::projection_matrix() const 
{ 
    return m_projection_mat; 
} 

const glm::mat4& engine::perspective_camera::view_matrix() const 
{ 
    return m_view_mat; 
} 

const glm::mat4& engine::perspective_camera::view_projection_matrix() const 
{ 
    return m_view_projection_mat; 
}

void engine::perspective_camera::process_mouse(float mouse_delta_x, float mouse_delta_y, bool constrain_pitch /*= true*/)
{
    // Smoothen the values
    mouse_delta_x *= s_mouse_sensitivity;
    mouse_delta_y *= s_mouse_sensitivity;

    m_yaw   += mouse_delta_x;
    m_pitch += mouse_delta_y;

    // Make sure that when pitch is out of bounds, screen doesn't get flipped
    if(constrain_pitch)
    {
        static const float pitch_limit = 89.0f;
        if(m_pitch > pitch_limit)
            m_pitch = pitch_limit;
        if(m_pitch < -pitch_limit)
            m_pitch = -pitch_limit;
    }
}

void engine::perspective_camera::move(e_direction direction, timestep ts) 
{
    if(direction == forward)

        m_position += s_movement_speed * ts * glm::vec3(m_front_vector.x, 0,m_front_vector.z);
    else if(direction == backward) 
        m_position -= s_movement_speed * ts * glm::vec3(m_front_vector.x, 0, m_front_vector.z);

    if(direction == left) 
        m_position -= s_movement_speed * ts * sprint_speed* glm::vec3(m_right_vector.x, 0, m_right_vector.z);
    else if(direction == right) 
        m_position += s_movement_speed * ts * sprint_speed* glm::vec3(m_right_vector.x, 0, m_right_vector.z);

    //LOG_CORE_TRACE("3d cam position: [{},{},{}]", m_position.x, m_position.y, m_position.z);
}

void engine::perspective_camera::move_rail(e_direction direction, timestep ts)
{
    if (direction == down)
        m_position += s_movement_speed * ts * m_up_vector;
    else if (direction == up)
        m_position -= s_movement_speed * ts * m_up_vector;

    if (direction == left)
        m_position -= s_movement_speed * ts * m_right_vector;
    else if (direction == right)
        m_position += s_movement_speed * ts * m_right_vector;

    //LOG_CORE_TRACE("3d cam position: [{},{},{}]", m_position.x, m_position.y, m_position.z); 
}

void engine::perspective_camera::rotate(e_rotation rotation, e_axis rotation_axis, timestep ts) 
{ 
    float* angle_ref = nullptr; 
    switch (rotation_axis) 
    { 
        case x: angle_ref = &m_rotation_angle.x; break; 
        case y: angle_ref = &m_rotation_angle.y; break; 
        case z: angle_ref = &m_rotation_angle.z; break; 
    } 

    ENGINE_ASSERT(angle_ref, "angle ptr is null!") 

        float& angle = *angle_ref; 
    if(rotation == anticlock_wise) 
    { 
        angle += s_rotation_speed * ts; 
        if(angle > 360.f) 
        { 
            angle = 0.f; 
            LOG_CORE_TRACE("A"); 
        } 
    } 
    else if(rotation == clock_wise) 
    { 
        angle -= s_rotation_speed * ts; 
        if(angle < 0.f) 
        { 
            angle = 360.f; 
            LOG_CORE_TRACE("B"); 
        } 
    } 

    //LOG_CORE_TRACE("after - 3d cam rotation: [{},{},{}]", m_rotation_angle.x, m_rotation_angle.y, m_rotation_angle.z); 
} 

void engine::perspective_camera::update_view_matrix() 
{ 
    //glm::mat4 transform{1}; 
    //transform = glm::rotate(transform, glm::radians(m_rotation_angle.x), {1,0,0}); 
    //transform = glm::rotate(transform, glm::radians(m_rotation_angle.y), {0,1,0}); 
    //transform = glm::rotate(transform, glm::radians(m_rotation_angle.z), {0,0,1}); 
    //transform = glm::translate(transform, m_position); 

    // inverting the transform matrix  
    //m_view_mat = glm::inverse(transform);
    if (!first_person_view) {
        m_view_mat = glm::lookAt(shell_position, shell_position + (glm::vec3(0.1f, 0.f, 0.f)-glm::vec3(0.f,20.f,0.f)), glm::vec3(1.f, 0.f, 0.f));
    }
    else {
        m_view_mat = glm::lookAt(m_position, m_position + m_front_vector, m_up_vector);
    }
    
    //MAKE AN SELECTOR BASED ON WHAT IS NEEDED
   //
   //
   // 
    //m_view_mat = glm::lookAt(glm::vec3(0.f, 20.f, 0.f), glm::vec3(0.1f, 0.f, 0.f), glm::vec3(0.f, 0.0f, 1.f));
    m_view_projection_mat = m_projection_mat * m_view_mat; 
}

void engine::perspective_camera::set_view_matrix(glm::vec3 position, glm::vec3 look_at)
{
	m_position = position;
	m_front_vector = glm::normalize(look_at - position);
	m_view_mat = glm::lookAt(m_position, m_position + m_front_vector, m_up_vector);
	m_view_projection_mat = m_projection_mat * m_view_mat;
}

void engine::perspective_camera::swap_view() {
    if (first_person_view) { first_person_view = false; }
    else if (!first_person_view) { first_person_view = true; }
}

void engine::perspective_camera::update_camera_vectors()
{
    // Calculate the new Front vector
    glm::vec3 front(0.f);
    float yaw_radians = glm::radians(m_yaw);
    float pitch_radians = glm::radians(m_pitch);
    front.x = cos(yaw_radians) * cos(pitch_radians);
    front.y = sin(pitch_radians);
    front.z = sin(yaw_radians) * cos(pitch_radians);

    m_front_vector = glm::normalize(front);
    // Also re-calculate the Right and Up vector
    // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    m_right_vector = glm::normalize(glm::cross(m_front_vector, m_world_up_vector));  
    m_up_vector   = glm::normalize(glm::cross(m_right_vector, m_front_vector));
    update_view_matrix();
}
