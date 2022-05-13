#include "Utils.h"
#include <ngl/Mat4.h>

ngl::Mat4 Utils::getTranspose(const ngl::Mat4 &_m)
{
    ngl::Mat4 transpose;
    transpose.m_30 = _m.m_30;
    transpose.m_31 = _m.m_31;
    transpose.m_32 = _m.m_32;

    return transpose;
}

ngl::Mat4 Utils::getScaleRotate( const ngl::Mat4 & _m)
{
    auto scale_rot = _m;
    scale_rot.m_30 = 0;
    scale_rot.m_31 = 0;
    scale_rot.m_32 = 0;

    return scale_rot;
}

const void Utils::printMatrix( const ngl::Mat4 & _m)
{
    std::cout << "\n";
    std::cout << '\n' << _m.m_00 << ' ' << _m.m_10 << ' '<< _m.m_20 << ' '<< _m.m_30 ;
    std::cout << '\n' << _m.m_01 << ' ' << _m.m_11 << ' '<< _m.m_21 << ' '<< _m.m_31 ;
    std::cout << '\n' << _m.m_02 << ' ' << _m.m_12 << ' '<< _m.m_22 << ' '<< _m.m_32 ;
    std::cout << '\n' << _m.m_03 << ' ' << _m.m_13 << ' '<< _m.m_23 << ' '<< _m.m_33 ;
    std::cout << "\n";
}