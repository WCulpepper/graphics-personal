/**
 * @file ArcballCam.hpp
 * @brief Abstract Camera class
 * @author Dr. Jeffrey Paone
 *
 * @copyright MIT License Copyright (c) 2021 Dr. Jeffrey Paone
 *
 *	These functions, classes, and constants help minimize common
 *	code that needs to be written.
 */
#include "Camera.hpp"
/// \desc concrete CSCI441::Camera implementation to implement an ArcBall camera model
/// \note camera direction is controlled by setting theta & phi and rotating the camera
class ArcballCam : public Camera {
public:
    explicit ArcballCam(GLfloat minRadius = 2.0f, GLfloat maxRadius = 30.0f) : _minRadius(minRadius), _maxRadius(maxRadius) {}

    /// \desc converts spherical theta & phi to cartesian x,y,z direction vector
    /// \note sets the camera's position to be on a sphere centered
    /// at the camera's look at position.  internally sets the
    /// camera's view matrix
    void recomputeOrientation() final;

    /// \desc updates the camera's position by decreasing the camera's radius
    /// \param movementFactor distance factor to scale the movement step
    /// \note internally sets the camera's view matrix
    void moveForward(GLfloat movementFactor) final;

    /// \desc updates the camera's position by increasing the camera's radius
    /// \param movementFactor distance factor to scale the movement step
    /// \note internally sets the camera's view matrix
    void moveBackward(GLfloat movementFactor) final;

private:
    /// \desc updates the camera position and recalculates the view matrix
    void _updateFreeCameraViewMatrix();

    GLfloat _minRadius;
    GLfloat _maxRadius;
};

inline void ArcballCam::recomputeOrientation() {
    // compute direction vector based on spherical to cartesian conversion
    _direction.x =  sinf( _theta )*sinf( _phi ) * _radius;
    _direction.y = -cosf( _phi ) * _radius;
    _direction.z = -cosf( _theta )*sinf( _phi ) * _radius;

    _updateFreeCameraViewMatrix();
}

inline void ArcballCam::moveForward(GLfloat movementFactor) {
    _radius -= movementFactor;

    // do not let our camera get too close
    if( _radius < _minRadius )  _radius = _minRadius;
    // do not let our camera get too far away
    if( _radius > _maxRadius ) _radius = _maxRadius;

    recomputeOrientation();
}

inline void ArcballCam::moveBackward(GLfloat movementFactor) {
    _radius += movementFactor;

    // do not let our camera get too close
    if( _radius < _minRadius )  _radius = _minRadius;
    // do not let our camera get too far away
    if( _radius > _maxRadius ) _radius = _maxRadius;

    recomputeOrientation();
}

inline void ArcballCam::_updateFreeCameraViewMatrix() {
    setPosition( _lookAtPoint + _direction );
    computeViewMatrix();
}