
#ifndef RVO_VECTOR3_H_
#define RVO_VECTOR3_H_

#include <cmath>
#include <cstddef>

#ifndef _NOIOSTREAM
#include <ostream>
#endif

namespace RVO {
	/**
	 * \brief  Defines a three-dimensional vector.
	 */
	class Vector3 {
	public:
		/**
		 * \brief   Constructs and initializes a three-dimensional vector instance to zero.
		 */
		inline Vector3()
		{
			val_[0] = 0.0f;
			val_[1] = 0.0f;
			val_[2] = 0.0f;
		}

		/**
		 * \brief   Constructs and initializes a three-dimensional vector from the specified three-dimensional vector.
		 * \param   vector  The three-dimensional vector containing the xyz-coordinates.
		 */
		inline Vector3(const Vector3 &vector)
		{
			val_[0] = vector[0];
			val_[1] = vector[1];
			val_[2] = vector[2];
		}

		/**
		 * \brief   Constructs and initializes a three-dimensional vector from the specified three-element array.
		 * \param   val  The three-element array containing the xyz-coordinates.
		 */
		inline explicit Vector3(const float val[3])
		{
			val_[0] = val[0];
			val_[1] = val[1];
			val_[2] = val[2];
		}

		/**
		 * \brief   Constructs and initializes a three-dimensional vector from the specified xyz-coordinates.
		 * \param   x  The x-coordinate of the three-dimensional vector.
		 * \param   y  The y-coordinate of the three-dimensional vector.
		 * \param   z  The z-coordinate of the three-dimensional vector.
		 */
		inline Vector3(float x, float y, float z)
		{
			val_[0] = x;
			val_[1] = y;
			val_[2] = z;
		}

		/**
		 * \brief   Returns the x-coordinate of this three-dimensional vector.
		 * \return  The x-coordinate of the three-dimensional vector.
		 */
		inline float x() const { return val_[0]; }

		/**
		 * \brief   Returns the y-coordinate of this three-dimensional vector.
		 * \return  The y-coordinate of the three-dimensional vector.
		 */
		inline float y() const { return val_[1]; }

		/**
		 * \brief   Returns the z-coordinate of this three-dimensional vector.
		 * \return  The z-coordinate of the three-dimensional vector.
		 */
		inline float z() const { return val_[2]; }

		/**
		 * \brief   Returns the specified coordinate of this three-dimensional vector.
		 * \param   i  The coordinate that should be returned (0 <= i < 3).
		 * \return  The specified coordinate of the three-dimensional vector.
		 */
		inline float operator[](size_t i) const { return val_[i]; }

		/**
		 * \brief   Returns a reference to the specified coordinate of this three-dimensional vector.
		 * \param   i  The coordinate to which a reference should be returned (0 <= i < 3).
		 * \return  A reference to the specified coordinate of the three-dimensional vector.
		 */
		inline float &operator[](size_t i) { return val_[i]; }

		/**
		 * \brief   Computes the negation of this three-dimensional vector.
		 * \return  The negation of this three-dimensional vector.
		 */
		inline Vector3 operator-() const
		{
			return Vector3(-val_[0], -val_[1], -val_[2]);
		}

		/**
		 * \brief   Computes the dot product of this three-dimensional vector with the specified three-dimensional vector.
		 * \param   vector  The three-dimensional vector with which the dot product should be computed.
		 * \return  The dot product of this three-dimensional vector with a specified three-dimensional vector.
		 */
		inline float operator*(const Vector3 &vector) const
		{
			return val_[0] * vector[0] + val_[1] * vector[1] + val_[2] * vector[2];
		}

		/**
		 * \brief   Computes the scalar multiplication of this three-dimensional vector with the specified scalar value.
		 * \param   scalar  The scalar value with which the scalar multiplication should be computed.
		 * \return  The scalar multiplication of this three-dimensional vector with a specified scalar value.
		 */
		inline Vector3 operator*(float scalar) const
		{
			return Vector3(val_[0] * scalar, val_[1] * scalar, val_[2] * scalar);
		}

		/**
		 * \brief   Computes the scalar division of this three-dimensional vector with the specified scalar value.
		 * \param   scalar  The scalar value with which the scalar division should be computed.
		 * \return  The scalar division of this three-dimensional vector with a specified scalar value.
		 */
		inline Vector3 operator/(float scalar) const
		{
			const float invScalar = 1.0f / scalar;

			return Vector3(val_[0] * invScalar, val_[1] * invScalar, val_[2] * invScalar);
		}

		/**
		 * \brief   Computes the vector sum of this three-dimensional vector with the specified three-dimensional vector.
		 * \param   vector  The three-dimensional vector with which the vector sum should be computed.
		 * \return 	The vector sum of this three-dimensional vector with a specified three-dimensional vector.
		 */
		inline Vector3 operator+(const Vector3 &vector) const
		{
			return Vector3(val_[0] + vector[0], val_[1] + vector[1], val_[2] + vector[2]);
		}

		/**
		 * \brief   Computes the vector difference of this three-dimensional vector with the specified three-dimensional vector.
		 * \param   vector  The three-dimensional vector with which the vector difference should be computed.
		 * \return  The vector difference of this three-dimensional vector with a specified three-dimensional vector.
		 */
		inline Vector3 operator-(const Vector3 &vector) const
		{
			return Vector3(val_[0] - vector[0], val_[1] - vector[1], val_[2] - vector[2]);
		}

		/**
		 * \brief   Tests this three-dimensional vector for equality with the specified three-dimensional vector.
		 * \param   vector  The three-dimensional vector with which to test for equality.
		 * \return  True if the three-dimensional vectors are equal.
		 */
		inline bool operator==(const Vector3 &vector) const
		{
			return val_[0] == vector[0] && val_[1] == vector[1] && val_[2] == vector[2];
		}

		/**
		 * \brief   Tests this three-dimensional vector for inequality with the specified three-dimensional vector.
		 * \param   vector  The three-dimensional vector with which to test for inequality.
		 * \return  True if the three-dimensional vectors are not equal.
		 */
		inline bool operator!=(const Vector3 &vector) const
		{
			return val_[0] != vector[0] || val_[1] != vector[1] || val_[2] != vector[2];
		}

		/**
		 * \brief   Sets the value of this three-dimensional vector to the scalar multiplication of itself with the specified scalar value.
		 * \param   scalar  The scalar value with which the scalar multiplication should be computed.
		 * \return  A reference to this three-dimensional vector.
		 */
		inline Vector3 &operator*=(float scalar)
		{
			val_[0] *= scalar;
			val_[1] *= scalar;
			val_[2] *= scalar;

			return *this;
		}

		/**
		 * \brief   Sets the value of this three-dimensional vector to the scalar division of itself with the specified scalar value.
		 * \param   scalar  The scalar value with which the scalar division should be computed.
		 * \return  A reference to this three-dimensional vector.
		 */
		inline Vector3 &operator/=(float scalar)
		{
			const float invScalar = 1.0f / scalar;

			val_[0] *= invScalar;
			val_[1] *= invScalar;
			val_[2] *= invScalar;

			return *this;
		}

		/**
		 * \brief   Sets the value of this three-dimensional vector to the vector
		 *             sum of itself with the specified three-dimensional vector.
		 * \param   vector  The three-dimensional vector with which the vector sum should be computed.
		 * \return  A reference to this three-dimensional vector.
		 */
		inline Vector3 &operator+=(const Vector3 &vector)
		{
			val_[0] += vector[0];
			val_[1] += vector[1];
			val_[2] += vector[2];

			return *this;
		}

		/**
		 * \brief   Sets the value of this three-dimensional vector to the vector difference of itself with the specified three-dimensional vector.
		 * \param   vector  The three-dimensional vector with which the vector difference should be computed.
		 * \return  A reference to this three-dimensional vector.
		 */
		inline Vector3 &operator-=(const Vector3 &vector)
		{
			val_[0] -= vector[0];
			val_[1] -= vector[1];
			val_[2] -= vector[2];

			return *this;
		}

	private:
		float val_[3];
	};


	/**
	 * \relates  Vector3
	 * \brief    Computes the scalar multiplication of the specified three-dimensional vector with the specified scalar value.
	 * \param    scalar  The scalar value with which the scalar multiplication should be computed.
	 * \param    vector  The three-dimensional vector with which the scalar multiplication should be computed.
	 * \return   The scalar multiplication of the three-dimensional vector with the scalar value.
	 */
	inline Vector3 operator*(float scalar, const Vector3 &vector)
	{
		return Vector3(scalar * vector[0], scalar * vector[1], scalar * vector[2]);
	}

	/**
	 * \relates  Vector3
	 * \brief    Computes the cross product of the specified three-dimensional vectors.
	 * \param    vector1  The first vector with which the cross product should be computed.
	 * \param    vector2  The second vector with which the cross product should be computed.
	 * \return   The cross product of the two specified vectors.
	 */
	inline Vector3 cross(const Vector3 &vector1, const Vector3 &vector2)
	{
		return Vector3(vector1[1] * vector2[2] - vector1[2] * vector2[1], vector1[2] * vector2[0] - vector1[0] * vector2[2], vector1[0] * vector2[1] - vector1[1] * vector2[0]);
	}

#ifndef _NOIOSTREAM
	/**
	 * \relates  Vector3
	 * \brief    Inserts the specified three-dimensional vector into the specified output stream.
	 * \param    os      The output stream into which the three-dimensional vector should be inserted.
	 * \param    vector  The three-dimensional vector which to insert into the output stream.
	 * \return   A reference to the output stream.
	 */
	inline std::ostream &operator<<(std::ostream &os, const Vector3 &vector)
	{
		os << "(" << vector[0] << "," << vector[1] << "," << vector[2] << ")";

		return os;
	}
#endif

	/**
	 * \relates  Vector3
	 * \brief    Computes the length of a specified three-dimensional vector.
	 * \param    vector  The three-dimensional vector whose length is to be computed.
	 * \return   The length of the three-dimensional vector.
	 */
	inline float abs(const Vector3 &vector)
	{
		return std::sqrt(vector * vector);
	}

	/**
	 * \relates  Vector3
	 * \brief    Computes the squared length of a specified three-dimensional vector.
	 * \param    vector  The three-dimensional vector whose squared length is to be computed.
	 * \return   The squared length of the three-dimensional vector.
	 */
	inline float absSq(const Vector3 &vector)
	{
		return vector * vector;
	}

	/**
	 * \relates  Vector3
	 * \brief    Computes the normalization of the specified three-dimensional vector.
	 * \param    vector  The three-dimensional vector whose normalization is to be computed.
	 * \return   The normalization of the three-dimensional vector.
	 */
	inline Vector3 normalize(const Vector3 &vector)
	{
		return vector / abs(vector);
	}
}

#endif
