
#include "Transform.h"

Transform::Transform()
    : m(Mat4(1.0)), mInv(Mat4(1)), forward(Vec3(0, 0, 1)), up(Vec3(0, -1, 0)), right(Vec3(-1, 0, 0)) {
    Decompose();
    UpdateDirections();
};

Transform::Transform(const Mat4& m)
    : m(m), mInv(glm::inverse(m)), forward(Vec3(0, 0, 1)), up(Vec3(0, -1, 0)), right(Vec3(-1, 0, 0)) {
    Decompose();
    UpdateDirections();
}

Transform::Transform(const Mat4& m, const Mat4& mInv)
    : m(m), mInv(mInv), forward(Vec3(0, 0, 1)), up(Vec3(0, 1, 0)), right(Vec3(-1, 0, 0)) {
    Decompose();
    UpdateDirections();
}

const Mat4& Transform::GetMatrix() const {
    return m;
}

const Mat4& Transform::GetInverseMatrix() const {
    return mInv;
}

void Transform::UpdateMatrices() {
    Mat4 mTranslate = glm::translate(position);
    Mat4 mScale = glm::scale(scale);
    Mat4 mRotate = glm::rotate(PiOver2, rotation);
    m = mTranslate * mScale * mRotate;
    mInv = glm::inverse(m);
    Decompose();
    UpdateDirections();
}

Vec3 Transform::ApplyPoint(Vec3 p) const {
    Float xp = m[0][0] * p.x + m[0][1] * p.y + m[0][2] * p.z + m[0][3];
    Float yp = m[1][0] * p.x + m[1][1] * p.y + m[1][2] * p.z + m[1][3];
    Float zp = m[2][0] * p.x + m[2][1] * p.y + m[2][2] * p.z + m[2][3];
    Float wp = m[3][0] * p.x + m[3][1] * p.y + m[3][2] * p.z + m[3][3];
    if (wp == 1.0f) {
        return Vec3(xp, yp, zp);
    }
    else {
        return Vec3(xp, yp, zp) / wp;
    }
}

Vec3 Transform::ApplyVector(Vec3 v) const {
    return Vec3(
        m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z,
        m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z,
        m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z);
}

Vec3 Transform::ApplyNormal(Vec3 n) const {
    Float x = n.x, y = n.y, z = n.z;
    return Vec3(
        mInv[0][0] * x + mInv[1][0] * y + mInv[2][0] * z,
        mInv[0][1] * x + mInv[1][1] * y + mInv[2][1] * z,
        mInv[0][2] * x + mInv[1][2] * y + mInv[2][2] * z);
}

Ray Transform::ApplyRay(const Ray& r, Float* tMax) const {
    Vec3 o = (*this).ApplyPoint(Vec3(r.origin));
    Vec3 d = (*this).ApplyVector(r.direction);

    Float lengthSquared = length2(d);
    if (lengthSquared > 0) {
        o += d * MachineEpsilon;
    }

    return Ray(r.x, r.y, o, d);
}

SurfaceInteraction Transform::ApplyInteraction(const SurfaceInteraction& in) const {
    SurfaceInteraction ret(in);
    ret.position = (*this).ApplyPoint(in.position);
    ret.normal = (*this).ApplyNormal(in.normal);
    if (length2(ret.normal) > 0.0f) {
        ret.normal = glm::normalize(ret.normal);
    }
    ret.wo = (*this).ApplyVector(in.wo);
    if (length2(ret.wo) > 0) {
        ret.wo = glm::normalize(ret.wo);
    }
    return ret;
}

Bounds3f Transform::ApplyBounds(const Bounds3f& b) const {
    Bounds3f bt;
    for (int32_t i = 0; i < 8; ++i) {
        bt = Union(bt, (*this).ApplyBounds(b));
    }
    return bt;
}

Vec3 Transform::ApplyInversePoint(Vec3 p) const {
    Float x = p.x, y = p.y, z = p.z;
    Float xp = (mInv[0][0] * x + mInv[0][1] * y) + (mInv[0][2] * z + mInv[0][3]);
    Float yp = (mInv[1][0] * x + mInv[1][1] * y) + (mInv[1][2] * z + mInv[1][3]);
    Float zp = (mInv[2][0] * x + mInv[2][1] * y) + (mInv[2][2] * z + mInv[2][3]);
    Float wp = (mInv[3][0] * x + mInv[3][1] * y) + (mInv[3][2] * z + mInv[3][3]);
    if (wp == 1.0f) {
        return Vec3(xp, yp, zp);
    }
    else {
        return Vec3(xp, yp, zp) / wp;
    }
}

Vec3 Transform::ApplyInverseVector(Vec3 v) const {
    Float x = v.x, y = v.y, z = v.z;
    return Vec3(
        mInv[0][0] * x + mInv[0][1] * y + mInv[0][2] * z,
        mInv[1][0] * x + mInv[1][1] * y + mInv[1][2] * z,
        mInv[2][0] * x + mInv[2][1] * y + mInv[2][2] * z);
}

Vec3 Transform::ApplyInverseNormal(Vec3 n) const {
    Float x = n.x, y = n.y, z = n.z;
    return Vec3(
        m[0][0] * x + m[1][0] * y + m[2][0] * z,
        m[0][1] * x + m[1][1] * y + m[2][1] * z,
        m[0][2] * x + m[1][2] * y + m[2][2] * z);
}

Ray Transform::ApplyInverseRay(const Ray& r, Float* tMax) const {
    Vec3 o = ApplyInversePoint(r.origin);
    Vec3 d = ApplyInverseVector(r.direction);

    Float lengthSquared = length2(d);
    if (lengthSquared > 0.0f) {
        o -= d * MachineEpsilon;
    }

    return Ray(r.x, r.y,o, d);
}

SurfaceInteraction Transform::ApplyInverseInteraction(const SurfaceInteraction& in) const {
    SurfaceInteraction ret(in);
    Transform t = Inverse(*this);
    ret.position = t.ApplyPoint(in.position);
    ret.normal = t.ApplyNormal(in.normal);
    if (length2(ret.normal) > 0.0f) {
        ret.normal = glm::normalize(ret.normal);
    }
    ret.wo = t.ApplyVector(in.wo);
    if (length2(ret.wo) > 0.0f) {
        ret.wo = glm::normalize(ret.wo);
    }
    return ret;
}

void Transform::Decompose() {
    glm::vec3 skew;
    glm::vec4 perspective;
    glm::quat qRotation;
    glm::decompose(m, scale, qRotation, position, skew, perspective);
    rotation = glm::eulerAngles(qRotation);
}

void Transform::UpdateDirections() {

}

bool Transform::IsIdentity() const {
    return m == Mat4();
}

bool Transform::HasScale(Float tolerance) const {
    Float la2 = length2((*this).ApplyVector(Vec3(1, 0, 0)));
    Float lb2 = length2((*this).ApplyVector(Vec3(0, 1, 0)));
    Float lc2 = length2((*this).ApplyVector(Vec3(0, 0, 1)));
    return (std::abs(la2 - 1) > tolerance || std::abs(lb2 - 1) > tolerance || std::abs(lc2 - 1) > tolerance);
}

bool Transform::SwapsHandedness() const {
    glm::mat3 s(
        m[0][0], m[0][1], m[0][2],
        m[1][0], m[1][1], m[1][2],
        m[2][0], m[2][1], m[2][2]);
    return glm::determinant(s) < 0;
}

bool Transform::operator==(const Transform& t) const {
    return t.m == m;
}

bool Transform::operator!=(const Transform& t) const {
    return t.m != m;
}

Transform Transform::operator*(const Transform& t2) const {
    return Transform(m * t2.m, t2.mInv * mInv);
}

Transform Inverse(const Transform& t) {
    return Transform(t.GetInverseMatrix(), t.GetMatrix());
}

Transform Transpose(const Transform& t) {
    return Transform(glm::transpose(t.GetMatrix()), glm::transpose(t.GetInverseMatrix()));
}

Transform RotateFromTo(Vec3 from, Vec3 to) {
    Vec3 refl;
    if (std::abs(from.x) < 0.72f && std::abs(to.x) < 0.72f) {
        refl = Vec3(1, 0, 0);
    }
    else if (std::abs(from.y) < 0.72f && std::abs(to.y) < 0.72f) {
        refl = Vec3(0, 1, 0);
    }
    else {
        refl = Vec3(0, 0, 1);
    }

    Vec3 u = refl - from, v = refl - to;
    Mat4 r;
    for (int32_t i = 0; i < 3; ++i) {
        for (int32_t j = 0; j < 3; ++j) {
            r[i][j] = ((i == j) ? 1 : 0) - 2 / glm::dot(u, u) * u[i] * u[j] -
                2 / glm::dot(v, v) * v[i] * v[j] +
                4 * glm::dot(u, v) / (glm::dot(u, u) * glm::dot(v, v)) * v[i] * u[j];
        }
    }

    return Transform(r, glm::transpose(r));
}
