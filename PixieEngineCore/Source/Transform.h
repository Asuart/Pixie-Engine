#pragma once
#include "RTMath.h"
#include "Ray.h"
#include "Interaction.h"
#include "Bounds.h"

class Transform {
	Mat4 m = Mat4(), mInv = Mat4();
public:
	Transform();
	Transform(const Mat4& m);
	Transform(const Mat4& m, const Mat4& mInv);

	const Mat4& GetMatrix() const;
	const Mat4& GetInverseMatrix() const;

	Vec3 ApplyPoint(Vec3 p) const;
	Vec3 ApplyVector(Vec3 v) const;
	Vec3 ApplyNormal(Vec3 n) const;
	Ray ApplyRay(const Ray& r, Float* tMax = nullptr) const;
	SurfaceInteraction ApplyInteraction(const SurfaceInteraction& in) const;
	Bounds3f ApplyBounds(const Bounds3f& b) const;

	Vec3 ApplyInversePoint(Vec3 p) const;
	Vec3 ApplyInverseVector(Vec3 v) const;
	Vec3 ApplyInverseNormal(Vec3 n) const;
	Ray ApplyInverseRay(const Ray& r, Float* tMax = nullptr) const;
	SurfaceInteraction ApplyInverseInteraction(const SurfaceInteraction& in) const;

	void Decompose(Vec3* T, Mat4* R, Mat4* S) const;
	bool IsIdentity() const;
	bool HasScale(Float tolerance = 1e-3f) const;
	bool SwapsHandedness() const;

	bool operator==(const Transform& t) const;
	bool operator!=(const Transform& t) const;
	Transform operator*(const Transform& t2) const;
};

Transform Inverse(const Transform& t);
Transform Transpose(const Transform& t);
Transform Rotate(Float sinTheta, Float cosTheta, Vec3 axis);
Transform Rotate(Float theta, Vec3 axis);
Transform RotateFromTo(Vec3 from, Vec3 to);
Transform Translate(Vec3 delta);
Transform Scale(Float x, Float y, Float z);
Transform RotateX(Float theta);
Transform RotateY(Float theta);
Transform RotateZ(Float theta);
Transform LookAt(Vec3 pos, Vec3 look, Vec3 up);
Transform Orthographic(Float zNear, Float zFar);
Transform Perspective(Float fov, Float n, Float f);
