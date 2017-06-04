#pragma once

#include "ITransform.h"
#include <glm/gtx/quaternion.hpp>

namespace ginkgo {

	class Transform : public ITransform
	{
	private:
		mutable mat4 matrix;
		vec3 dilation;
		vec3 translation;
			vec3 axis;
			float angle;
		mat4 identity;
		mutable bool matrixOOD;
	public:
		Transform()
		{
			axis = vec3(0, 1, 0);
			dilation = vec3(1, 1, 1);
			translation = vec3();
			angle = 0;
			matrixOOD = true;
			getMatrix();
		}

		const mat4& getMatrix() const override 
		{
			if (matrixOOD)
			{
				matrixOOD = false;
				matrix = rotate(identity, angle, axis) * translate(identity, translation) * glm::scale(identity, dilation);
			}
			return matrix; 
		}

		void setMatrix(const mat4& matrix) override { this->matrix = matrix; }
		
		void scaleMatrix(const vec3& scale) override
		{ 
			dilation = scale;
			matrixOOD = true;
		}

		void translateMatrix(const vec3& translation) override
		{ 
			this->translation = translation;
			matrixOOD = true;
		}

		void rotateMatrix(float angleInRadians, const vec3& rotation) override 
		{ 
			this->axis = rotation;
			this->angle = angleInRadians;
			matrixOOD = true;
		}

		vec3 const& getScale() const override
		{
			return dilation;
		}
		
		vec3 const& getTranslation() const override
		{
			return translation;
		}

		vec3 const& getAxis() const override
		{
			return axis;
		}

		float const& getAngle() const override
		{
			return angle;
		}
	};

}

/*
templated in the future for any kind of glm data type as needed
*/