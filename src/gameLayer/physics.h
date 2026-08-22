#pragma once
#include<raylib.h>

inline Vector2 operator+(const Vector2& a, const Vector2& b)
{
	return { a.x + b.x, a.y + b.y };
}
inline Vector2 operator-(const Vector2& a, const Vector2& b)
{
	return { a.x - b.x, a.y - b.y };
}
inline Vector2 operator*(const Vector2& a, float scalar)
{
	return { a.x * scalar, a.y * scalar };
}
inline Vector2 operator/(const Vector2& a, float scalar)
{
	return { a.x / scalar, a.y / scalar };
}
inline Vector2& operator*=(Vector2& a, float scalar)
{
	a.x *= scalar;
	a.y *= scalar;
	return a;
}
inline Vector2& operator/=(Vector2& a, float scalar)
{
	a.x /= scalar;
	a.y /= scalar;
	return a;
}
inline Vector2& operator+=(Vector2& a, float scalar)
{
	a.x += scalar;
	a.y += scalar;
	return a;
}
inline Vector2& operator-=(Vector2& a, float scalar)
{
	a.x -= scalar;
	a.y -= scalar;
	return a;
}
inline bool operator==(const Vector2& a, const Vector2& b)
{
	return(a.x == b.x && a.y == b.y);
}
inline bool operator!=(const Vector2& a, const Vector2& b)
{
	return !(a == b);
}
inline Vector2& operator+=(Vector2& a, const Vector2& b)
{
	a.x += b.x;
	a.y += b.y;
	return a;
}
inline Vector2& operator-=(Vector2& a, const Vector2& b)
{
	a.x -= b.x;
	a.y -= b.y;
	return a;
}
inline Vector2& operator*=(Vector2& a, const Vector2& b)
{
	a.x *= b.x;
	a.y *= b.y;
	return a;
}
inline Vector2& operator/=(Vector2& a, const Vector2& b)
{
	a.x /= b.x;
	a.y /= b.y;
	return a;
}


struct Transform2D
{
	Vector2 pos = {};
	float w = 0;
	float h = 0;

	Vector2 GetCenter() const { return{ pos.x, pos.y }; }
	Vector2 GetCenterLeft() const { return { pos.x - w * 0.5f , pos.y }; }
	Vector2 GetCenterRight() const { return { pos.x + w * 0.5f, pos.y }; }

	Vector2 GetButtomCenter() const { return {pos.x, pos.y + h * 0.5f}; }
	Vector2 GetBottomLeft() const { return{ pos.x - w * 0.5f, pos.y + h * 0.5f }; }
	Vector2 GetBottomRight() const { return{ pos.x + w * 0.5f, pos.y + h * 0.5f }; }

	Vector2 GetTopCenter() const { return{pos.x, pos.y - h * 0.5f}; }
	Vector2 GetTopLeft() const { return{pos.x - w * 0.5f, pos.y - h * 0.5f }; }
	Vector2 GetTopRight() const { return{ pos.x + w * 0.5f , pos.y - h * 0.5f }; }
	// 根据物体的中心点，获取左上位置坐标
	Rectangle getAABB()
	{
		return{ pos.x - w * 0.5f, pos.y - h * 0.5f, w, h };
	}

	bool intersectPoint(Vector2 point, float delta = 0)
	{
		Rectangle rec = getAABB();
		rec.x -= delta;
		rec.y -= delta;
		rec.width += 2 * delta;
		rec.height += 2 * delta;
		return CheckCollisionPointRec(point, rec);
	}

	bool intersectTrasndform(Transform2D other, float delta = 0)
	{
		Rectangle rec = getAABB();
		Rectangle rec_other = other.getAABB();
		rec.x -= delta;
		rec.y -= delta;
		rec.width += 2 * delta;
		rec.height += 2 * delta;

		rec_other.x -= delta;
		rec_other.y -= delta;
		rec_other.width += 2 * delta;
		rec_other.height += 2 * delta;
		return CheckCollisionRecs(rec_other, rec);
	}
};

struct PhysicalEntity
{
	Transform2D transform;
	Vector2 lastPosition = {};
	Vector2 velocity = {};
	Vector2 acceleration = {};

	// 瞬间移动，并同步上一帧位置
	void teleport(Vector2 pos)
	{
		transform.pos = pos;
		lastPosition = pos;
	}
	//根据加速度、速度更新位置，加入阻力
	void updateForces(float deltaTime)
	{
		velocity += acceleration * deltaTime;
		transform.pos += velocity * deltaTime;

		Vector2 dragVector = Vector2{ velocity.x * std::abs(velocity.x), velocity.y * std::abs(velocity.y) };
		float drag = 0.01f;
		if (Vector2Length(dragVector) * drag * deltaTime > Vector2Length(velocity))
		{
			velocity = {};
		}
		else
		{
			velocity -= dragVector * drag * deltaTime;
		}
		if (Vector2Length(velocity) < 0.01f)
		{
			velocity = {};
		}
		acceleration = {};
	}
	// 保存当前帧位置给下一帧使用
	void updateFinal()
	{
		lastPosition = { transform.pos.x, transform.pos.y };
	}
	// 添加重力加速度
	void addGravity()
	{
		acceleration = { 0 , 20.0f };
	}
};