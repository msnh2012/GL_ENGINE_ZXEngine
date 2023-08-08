#include "IntersectionDetector.h"
#include "CollisionPrimitive.h"

namespace ZXEngine
{
	namespace PhysZ
	{
		bool IntersectionDetector::Detect(const CollisionBox& box1, const CollisionBox& box2)
		{
			Vector3 centerLine = box1.mTransform.GetColumn(3) - box2.mTransform.GetColumn(3);

			// �����ᶨ��(������͹���岻�ཻ����һ�����ٴ���һ���ᣬʹ������͹����ͶӰ��������Ϻ�û���ص�)
			return 
				IsOverlapOnAxis(box1, box2, box1.mTransform.GetColumn(0), centerLine) &&
				IsOverlapOnAxis(box1, box2, box1.mTransform.GetColumn(1), centerLine) &&
				IsOverlapOnAxis(box1, box2, box1.mTransform.GetColumn(2), centerLine) &&

				IsOverlapOnAxis(box1, box2, box2.mTransform.GetColumn(0), centerLine) &&
				IsOverlapOnAxis(box1, box2, box2.mTransform.GetColumn(1), centerLine) &&
				IsOverlapOnAxis(box1, box2, box2.mTransform.GetColumn(2), centerLine) &&

				IsOverlapOnAxis(box1, box2, Math::Cross(box1.mTransform.GetColumn(0), box2.mTransform.GetColumn(0)), centerLine) &&
				IsOverlapOnAxis(box1, box2, Math::Cross(box1.mTransform.GetColumn(0), box2.mTransform.GetColumn(1)), centerLine) &&
				IsOverlapOnAxis(box1, box2, Math::Cross(box1.mTransform.GetColumn(0), box2.mTransform.GetColumn(2)), centerLine) &&
				IsOverlapOnAxis(box1, box2, Math::Cross(box1.mTransform.GetColumn(1), box2.mTransform.GetColumn(0)), centerLine) &&
				IsOverlapOnAxis(box1, box2, Math::Cross(box1.mTransform.GetColumn(1), box2.mTransform.GetColumn(1)), centerLine) &&
				IsOverlapOnAxis(box1, box2, Math::Cross(box1.mTransform.GetColumn(1), box2.mTransform.GetColumn(2)), centerLine) &&
				IsOverlapOnAxis(box1, box2, Math::Cross(box1.mTransform.GetColumn(2), box2.mTransform.GetColumn(0)), centerLine) &&
				IsOverlapOnAxis(box1, box2, Math::Cross(box1.mTransform.GetColumn(2), box2.mTransform.GetColumn(1)), centerLine) &&
				IsOverlapOnAxis(box1, box2, Math::Cross(box1.mTransform.GetColumn(2), box2.mTransform.GetColumn(2)), centerLine);
		}

		bool IntersectionDetector::Detect(const CollisionSphere& sphere1, const CollisionSphere& sphere2)
		{
			Vector3 centerLine = sphere1.mTransform.GetColumn(3) - sphere2.mTransform.GetColumn(3);
			return centerLine.GetMagnitudeSquared() < (sphere1.mRadius + sphere2.mRadius) * (sphere1.mRadius + sphere2.mRadius);
		}

		bool IntersectionDetector::Detect(const CollisionBox& box, const CollisionPlane& plane)
		{
			return DetectBoxAndHalfSpace(box, plane);
		}

		bool IntersectionDetector::Detect(const CollisionSphere& sphere, const CollisionPlane& plane)
		{
			return DetectSphereAndHalfSpace(sphere, plane);
		}

		bool IntersectionDetector::DetectBoxAndHalfSpace(const CollisionBox& box, const CollisionPlane& plane)
		{
			// ����Box��ƽ�淨���ϵ�ͶӰ����
			float projectedLength = box.GetHalfProjectedLength(plane.mNormal);
			// Box��ƽ��ľ���
			float distance = Math::Dot(plane.mNormal, box.mTransform.GetColumn(3)) - projectedLength;
			return distance <= plane.mDistance;
		}

		bool IntersectionDetector::DetectSphereAndHalfSpace(const CollisionSphere& sphere, const CollisionPlane& plane)
		{
			float distance = Math::Dot(plane.mNormal, sphere.mTransform.GetColumn(3)) - sphere.mRadius;
			return distance <= plane.mDistance;
		}

		bool IntersectionDetector::IsOverlapOnAxis(const CollisionBox& box1, const CollisionBox& box2, const Vector3& axis, const Vector3& centerLine)
		{
			float projectedLength1 = box1.GetHalfProjectedLength(axis);
			float projectedLength2 = box2.GetHalfProjectedLength(axis);

			float distance = fabsf(Math::Dot(axis, centerLine));

			return distance < projectedLength1 + projectedLength2;
		}

		float IntersectionDetector::GetPenetrationOnAxis(const CollisionBox& box1, const CollisionBox& box2, const Vector3& axis, const Vector3& centerLine)
		{
			float projectedLength1 = box1.GetHalfProjectedLength(axis);
			float projectedLength2 = box2.GetHalfProjectedLength(axis);

			float distance = fabsf(Math::Dot(axis, centerLine));

			return projectedLength1 + projectedLength2 - distance;
		}

		bool IntersectionDetector::DetectLineSegmentContact(
			const Vector3& midPoint1, const Vector3& dir1, float halfLength1,
			const Vector3& midPoint2, const Vector3& dir2, float halfLength2,
			Vector3& contactPoint, bool useOne)
		{
			// �����߶η��򳤶ȵ�ƽ��
			float squaredLength1 = dir1.GetMagnitudeSquared();
			float squaredLength2 = dir2.GetMagnitudeSquared();
			// �����߶η���ĵ��
			float dot_d1_d2 = Math::Dot(dir1, dir2);

			float denominator = squaredLength1 * squaredLength2 - dot_d1_d2 * dot_d1_d2;

			// ��������߶�ƽ�У���ôdot_d1_d2��ֵ��Ӧ������(�н�0)��(�н�180)dir1����*dir2����
			// ��ô(squaredLength1 * squaredLength2)��(dot_d1_d2 * dot_d1_d2)���ȼ���((dir1����*dir2����)^2)
			// �������denominator��ֵ��С������Ϊ�����߶�ƽ�У������޽���
			if (fabsf(denominator) < 0.0001f)
			{
				contactPoint = useOne ? midPoint1 : midPoint2;
				return false;
			}

			// ��2����1������
			Vector3 p2top1 = midPoint1 - midPoint2;
			// ��2����1�������������߶η���ĵ��
			float dot_2to1_d1 = Math::Dot(p2top1, dir1);
			float dot_2to1_d2 = Math::Dot(p2top1, dir2);

			// ���㵽�߶�1�е�ľ���
			float distance1 = (dot_d1_d2 * dot_2to1_d2 - squaredLength2 * dot_2to1_d1) / denominator;
			// ���㵽�߶�2�е�ľ���
			float distance2 = (squaredLength1 * dot_2to1_d2 - dot_d1_d2 * dot_2to1_d1) / denominator;

			// �������û��ͬʱ�������߶������ཻ
			if (distance1 > halfLength1 || distance1 < -halfLength1 || distance2 > halfLength2 || distance2 < -halfLength2)
			{
				contactPoint = useOne ? midPoint1 : midPoint2;
				return false;
			}
			else
			{
				// ͨ���߶�1���㽻��
				Vector3 contactPos1 = midPoint1 + dir1 * distance1;
				// ͨ���߶�2���㽻��
				Vector3 contactPos2 = midPoint2 + dir2 * distance2;
				// ���������ƽ��ֵ(�����������������Ӧ������ȫ�غϵģ�����ʵ�������л��������ܣ�����ȡƽ��)
				contactPoint = (contactPos1 + contactPos2) * 0.5f;
				return true;
			}
		}
	}
}