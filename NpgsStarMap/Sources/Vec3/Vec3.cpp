#include "Vec3.h"
Vec3 add_vectors(const Vec3& v1, const Vec3& v2) {
	return Vec3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}

Vec3 subtract_vectors(const Vec3& v1, const Vec3& v2) {
	return Vec3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}

Vec3 cross_product(const Vec3& v1, const Vec3& v2) {
	return Vec3(
		v1.y * v2.z - v1.z * v2.y,
		v1.z * v2.x - v1.x * v2.z,
		v1.x * v2.y - v1.y * v2.x
	);
}

double dot_product(const Vec3& v1, const Vec3& v2) {
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

Vec3 normalize_vector(const Vec3& v) {
	double length = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
	if (length == 0) {
		throw std::runtime_error("Cannot normalize zero vector");
	}
	return Vec3(v.x / length, v.y / length, v.z / length);
}

double vector_length(const Vec3& v) {
	return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}
Vec3 scalar_multiply(double scalar, const Vec3& v) {
	return Vec3(scalar * v.x, scalar * v.y, scalar * v.z);
}