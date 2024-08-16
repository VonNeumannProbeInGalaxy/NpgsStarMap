#pragma once
struct Vec3 {
	double x, y, z;
	Vec3(double x = 0, double y = 0, double z = 0) : x(x), y(y), z(z) {}
};
Vec3 add_vectors(const Vec3& v1, const Vec3& v2);
Vec3 subtract_vectors(const Vec3& v1, const Vec3& v2);
Vec3 cross_product(const Vec3& v1, const Vec3& v2);
double dot_product(const Vec3& v1, const Vec3& v2);
Vec3 normalize_vector(const Vec3& v);
double vector_length(const Vec3& v);
Vec3 scalar_multiply(double scalar, const Vec3& v);