﻿#include <Siv3D.hpp> // OpenSiv3D v0.4.1

struct player_ball {
	Circle circle;
	Color color;
	Vec2 velocity;
	Vec2 before_pos;
	int jump_frame;
	int gravity_frame;

	player_ball() {
		before_pos = circle.center = Vec2(350, 480);
		circle.r = 20;
		color = Palette::Orange;
		jump_frame = 0;
		gravity_frame = 0;
	}

	void move(Array<Rect>& scaffold, Array<Circle>& jump_item, Array<Circle>& gravity_item, double& camera_pos) {
		if (Cursor::PosF().x < circle.x) {
			velocity.x -= 0.2;
		}
		else {
			velocity.x += 0.2;
		}
		if (circle.x < 120) {
			circle.x = 120;
			velocity.x *= -0.3;
		}
		if (circle.x > 580) {
			circle.x = 580;
			velocity.x *= -0.3;
		}
		velocity.x *= 0.95;
		if (gravity_frame) {
			gravity_frame--;
			gravity(0.3);
		}
		else {
			gravity(0.45);
		}
		for (Rect& r : scaffold) {
			if (r.intersects(circle) && before_pos.y <= r.y - circle.r) {
				circle.center.y = r.y - circle.r;
				velocity = Vec2(0, 0);
				if (MouseL.down()) {
					jump_frame = 8;
				}
			}
		}
		for (int i : step(jump_item.size())) {
			if (jump_item[i].intersects(circle)) {
				jump(25);
				jump_item.erase(jump_item.begin() + i);
				break;
			}
		}		
		for (int i : step(gravity_item.size())) {
			if (gravity_item[i].intersects(circle)) {
				gravity_frame = 300;
				//velocity.y = -10;
				gravity_item.erase(gravity_item.begin() + i);
				break;
			}
		}
		if (jump_frame && MouseL.pressed()) {
			jump(5 - (9 - jump_frame) * 0.5);
			jump_frame--;
		}
		else {
			jump_frame = 0;
		}
		if (camera_pos + circle.y < 300) {
			camera_pos = 300 - circle.y;
		}
		if (camera_pos + circle.y > 500) {
			camera_pos = 500 - circle.y;
		}
		before_pos = circle.center;
		velocity.y = Max(-30.0, velocity.y);
		velocity.y = Min(30.0, velocity.y);
		circle.center += velocity;
	}
	void draw(double& camera_pos) {
		circle.movedBy(0,camera_pos).draw(color);
	}
	void gravity(double d) {
		velocity.y += d;
	}
	void jump(double d) {
		velocity.y = Min(0.0, velocity.y);
		velocity.y -= d;
	}
};

void Main() {
	Scene::SetBackground(Color(0, 0, 50));
	Window::Resize(1000, 600);
	const Rect ground(100, 500, 500, 100);
	const Rect left_wall(0, 0, 100, 600);
	const Rect right_wall(600, 0, 50, 600);
	const Array<Color>color_palette{
		Color(200, 0, 0), Color(0, 200, 0), Color(100, 100, 200), Color(200, 200, 0), Color(200, 0, 200), Color(0, 200, 200)
	};
	Array<Rect>scaffold;
	Array<Circle>jump_item;
	Array<Circle>gravity_item;
	Array<std::pair<Polygon, Color>> star;
	player_ball ball;
	double camera_pos = 0;
	scaffold.push_back(Rect(100, 500, 500, 100));
	for (int i = -Random(400, 450); i <= 500000; i += Random(0, 200)) {
		star.push_back(std::make_pair(Shape2D::NStar(Random(5, 10), 7, 4, Vec2(Random(110, 590), -i)).asPolygon(), color_palette[Random(0, 5)]));
	}
	for (int i = -Random(300, 450); i <= 500000; i += Random(100, 200)) {
		int tmp = Random(100, 600);
		int left = Max(100, tmp - Random(50, 100));
		int right = Min(600, tmp + Random(50, 100));
		scaffold.push_back(Rect(left, -i, right - left, 10));
	}
	for (int i = Random(1000, 5000); i <= 500000; i += Random(1000, 5000)) {
		jump_item.push_back(Circle(Random(115, 585), -i, 15));
	}
	for (int i = Random(1000, 5000); i <= 500000; i += Random(1000, 5000)) {
		gravity_item.push_back(Circle(Random(115, 585), -i, 15));
	}
	while (System::Update()) {

		ball.move(scaffold, jump_item, gravity_item, camera_pos);
		for (std::pair<Polygon, Color>& p : star) {
			p.first.movedBy(0, camera_pos / 2).draw(p.second);
		}
		ball.draw(camera_pos);	

		for (Rect& r : scaffold) {
			r.movedBy(0, camera_pos).draw(Color(255, 255, 200));
		}
		for (Circle& c : jump_item) {
			c.movedBy(0, camera_pos).draw(Color(155, 255, 0));
		}		
		for (Circle& c : gravity_item) {
			c.movedBy(0, camera_pos).draw(Color(255, 50, 100));
		}
		ground.movedBy(0,camera_pos).draw(Color(0, 100, 20));
		left_wall.draw(Palette::Black);
		right_wall.draw(Palette::Black);
	}
}
