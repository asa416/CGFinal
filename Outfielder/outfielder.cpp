#include "outfielder.h"

Outfielder::Outfielder() : Pos(glm::vec3(0.0, 0.0, -4.0)), speed(0.05), height(0.2), weight(0.1), dressColor(glm::vec3(1.0, 0.0, 0.0))
{
	lr = 0.0;
	rr = 0.0;
	ratio = 1.0f;
	bodyMat = glm::translate(bodyMat, glm::vec3(Pos.x, 0.4, Pos.z));
	bodyMat = glm::scale(bodyMat, glm::vec3(0.1, 0.2, 0.1));
	headMat = glm::translate(headMat, glm::vec3(Pos.x, 0.65, Pos.z));
	headMat = glm::scale(headMat, glm::vec3(0.05, 0.06, 0.05));

	leftlegMat = glm::translate(leftlegMat, glm::vec3(Pos.x + 0.05, 0.1, Pos.z));
	leftlegMat = glm::scale(leftlegMat, glm::vec3(0.025, 0.1, 0.025));

	rightlegMat = glm::translate(rightlegMat, glm::vec3(Pos.x - 0.05, 0.1, Pos.z));
	rightlegMat = glm::scale(rightlegMat, glm::vec3(0.025, 0.1, 0.025));

	lefthandMat = glm::translate(lefthandMat, glm::vec3(Pos.x + 0.1, 0.45, Pos.z));
	lefthandMat = glm::scale(lefthandMat, glm::vec3(0.025, 0.1, 0.025));

	righthandMat = glm::translate(righthandMat, glm::vec3(Pos.x - 0.1, 0.45, Pos.z));
	righthandMat = glm::scale(righthandMat, glm::vec3(0.025, 0.1, 0.025));

	slideMat = glm::rotate(slideMat, glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));

	catching = false;
	sliding = false;
	slideCount = 5;
}

void Outfielder::Move(const glm::vec3& dir)
{
	glm::vec3 dist;
	if (sliding) {
		dist = dir * glm::vec3(2 * speed, 2 * speed, 2 * speed);
		Pos = Pos + dist;
		slideCount--;
	}
	else {
		dist = dir * glm::vec3(speed, speed, speed);
		Pos = Pos + dist;
	}

	float radian{};

	if (dist.x > 0.0) {
		if (dist.z == 0.0)
			radian = 90.0f;
		else if (dist.z > 0)
			radian = 45.0f;
		else
			radian = 135.0f;
	}
	else if (dist.x == 0.0) {
		if (dist.z == 0.0)
			radian = 0.0f;
		else if (dist.z > 0)
			radian = 0.0f;
		else
			radian = 180.0f;
	}
	else {
		if (dist.z == 0.0)
			radian = 270.0f;
		else if (dist.z > 0)
			radian = 315.0f;
		else
			radian = 225.0f;
	}

	rot = glm::mat4(1.0f);
	rot = glm::translate(rot, glm::vec3(Pos.x, Pos.y, Pos.z));
	rot = glm::rotate(rot, glm::radians(radian), glm::vec3(0.0f, 1.0f, 0.0f));
	rot = glm::translate(rot, glm::vec3(-Pos.x, -Pos.y, -Pos.z));

	glm::mat4 trans = glm::mat4(1.0f);
	trans = glm::translate(trans, dist);

	this->bodyMat = trans * this->bodyMat;
	this->headMat = trans * this->headMat;
	this->leftlegMat = trans * this->leftlegMat;
	this->rightlegMat = trans * this->rightlegMat;
	this->lefthandMat = trans * this->lefthandMat;
	this->righthandMat = trans * this->righthandMat;

	if (!sliding) {
		this->leftrotl = glm::mat4(1.0f);
		this->leftrotl = glm::translate(this->leftrotl, glm::vec3(Pos.x, 0.2, Pos.z));
		this->leftrotl = glm::rotate(this->leftrotl, glm::radians(5.0f * lr), glm::vec3(1.0f, 0.0f, 0.0f));
		this->leftrotl = glm::translate(this->leftrotl, glm::vec3(-Pos.x, -0.2, -Pos.z));

		this->rightrotl = glm::mat4(1.0f);
		this->rightrotl = glm::translate(this->rightrotl, glm::vec3(Pos.x, 0.2, Pos.z));
		this->rightrotl = glm::rotate(this->rightrotl, glm::radians(5.0f * rr), glm::vec3(1.0f, 0.0f, 0.0f));
		this->rightrotl = glm::translate(this->rightrotl, glm::vec3(-Pos.x, -0.2, -Pos.z));

		this->leftroth = glm::mat4(1.0f);
		this->leftroth = glm::translate(this->leftroth, glm::vec3(Pos.x, 0.45, Pos.z));
		this->leftroth = glm::rotate(this->leftroth, glm::radians(5.0f * -lr), glm::vec3(1.0f, 0.0f, 0.0f));
		this->leftroth = glm::translate(this->leftroth, glm::vec3(-Pos.x, -0.45, -Pos.z));

		this->rightroth = glm::mat4(1.0f);
		this->rightroth = glm::translate(this->rightroth, glm::vec3(Pos.x, 0.45, Pos.z));
		this->rightroth = glm::rotate(this->rightroth, glm::radians(5.0f * -rr), glm::vec3(1.0f, 0.0f, 0.0f));
		this->rightroth = glm::translate(this->rightroth, glm::vec3(-Pos.x, -0.45, -Pos.z));
		lr += ratio;
		rr -= ratio;
		if (lr > 5.0)
			ratio *= -1;
		if (lr < -5.0)
			ratio *= -1;
	}
	else {
		slideMat = glm::mat4(1.0f);
		slideMat = glm::translate(slideMat, glm::vec3(Pos.x, Pos.y, Pos.z));
		slideMat = glm::rotate(slideMat, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		slideMat = glm::translate(slideMat, glm::vec3(-Pos.x, -Pos.y, -Pos.z));
		if (slideCount < 0) {
			stopSlide();
		}
	}
}

glm::vec3 Outfielder::getColor() const
{
	return dressColor;
}

void Outfielder::draw(const std::shared_ptr<Shader>& shader, GLuint vao)
{
	if (!sliding) {
		glBindVertexArray(vao);
		(*shader).setVec3("objectColor", glm::vec3(0.9, 0.6, 0.5));
		(*shader).setMat4("model", rot * headMat);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		(*shader).setMat4("model", rot * leftrotl * leftlegMat);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		(*shader).setMat4("model", rot * rightrotl * rightlegMat);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		(*shader).setMat4("model", rot * rightroth * righthandMat);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		if (!catching) {
			(*shader).setMat4("model", rot * leftroth * lefthandMat);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		else {
			glm::mat4 left = glm::mat4(1.0f);
			left = glm::translate(left, glm::vec3(Pos.x, 0.65, Pos.z));
			left = glm::rotate(left, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			left = glm::translate(left, glm::vec3(-Pos.x, -0.45, -Pos.z));
			(*shader).setMat4("model", rot * left * lefthandMat);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		(*shader).setVec3("objectColor", dressColor);
		(*shader).setMat4("model", rot * bodyMat);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
	else {
		glBindVertexArray(vao);
		(*shader).setVec3("objectColor", glm::vec3(0.9, 0.6, 0.5));
		(*shader).setMat4("model", rot * slideMat * headMat);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		(*shader).setMat4("model", rot * slideMat * leftlegMat);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		(*shader).setMat4("model", rot * slideMat * rightlegMat);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		(*shader).setMat4("model", rot * slideMat * righthandMat);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glm::mat4 left = glm::mat4(1.0f);
		left = glm::translate(left, glm::vec3(Pos.x, 0.65, Pos.z));
		left = glm::rotate(left, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		left = glm::translate(left, glm::vec3(-Pos.x, -0.45, -Pos.z));
		(*shader).setMat4("model",rot * slideMat * left * lefthandMat);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		(*shader).setVec3("objectColor", dressColor);
		(*shader).setMat4("model", rot * slideMat * bodyMat);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
}

void Outfielder::draw_mini(const std::shared_ptr<Shader>& shader, GLuint vao)
{
	glm::mat4 sc = glm::mat4(1.0f);
	sc = glm::scale(sc, glm::vec3(2.0, 2.0, 2.0));

	glBindVertexArray(vao);
	(*shader).setMat4("model", rot * headMat * sc);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	(*shader).setMat4("model", rot * leftroth * lefthandMat * sc);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	(*shader).setMat4("model", rot * rightroth * righthandMat * sc);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	(*shader).setMat4("model", rot * leftrotl * leftlegMat * sc);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	(*shader).setMat4("model", rot * rightrotl * rightlegMat * sc);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	(*shader).setMat4("model", rot * bodyMat * sc);
	glDrawArrays(GL_TRIANGLES, 0, 36);

}

void Outfielder::setColor(const std::shared_ptr<Shader>& shader) {
	(*shader).setVec3("objectColor", dressColor);
}

glm::vec3 Outfielder::getPos() const
{
	return Pos;
}

void Outfielder::setCatch() {
	if (!catching)
		catching = true;
	else
		catching = false;
}

bool Outfielder::isSlide() {
	return sliding;
}

void Outfielder::setSlide() {
	sliding = true;
}

void Outfielder::stopSlide() {
	slideCount = 5;
	sliding = false;
	this->leftrotl = glm::mat4(1.0f);
	this->leftrotl = glm::translate(this->leftrotl, glm::vec3(Pos.x, 0.2, Pos.z));
	this->leftrotl = glm::rotate(this->leftrotl, glm::radians(5.0f * lr), glm::vec3(1.0f, 0.0f, 0.0f));
	this->leftrotl = glm::translate(this->leftrotl, glm::vec3(-Pos.x, -0.2, -Pos.z));

	this->rightrotl = glm::mat4(1.0f);
	this->rightrotl = glm::translate(this->rightrotl, glm::vec3(Pos.x, 0.2, Pos.z));
	this->rightrotl = glm::rotate(this->rightrotl, glm::radians(5.0f * rr), glm::vec3(1.0f, 0.0f, 0.0f));
	this->rightrotl = glm::translate(this->rightrotl, glm::vec3(-Pos.x, -0.2, -Pos.z));

	this->leftroth = glm::mat4(1.0f);
	this->leftroth = glm::translate(this->leftroth, glm::vec3(Pos.x, 0.45, Pos.z));
	this->leftroth = glm::rotate(this->leftroth, glm::radians(5.0f * -lr), glm::vec3(1.0f, 0.0f, 0.0f));
	this->leftroth = glm::translate(this->leftroth, glm::vec3(-Pos.x, -0.45, -Pos.z));

	this->rightroth = glm::mat4(1.0f);
	this->rightroth = glm::translate(this->rightroth, glm::vec3(Pos.x, 0.45, Pos.z));
	this->rightroth = glm::rotate(this->rightroth, glm::radians(5.0f * -rr), glm::vec3(1.0f, 0.0f, 0.0f));
	this->rightroth = glm::translate(this->rightroth, glm::vec3(-Pos.x, -0.45, -Pos.z));
	lr += ratio;
	rr -= ratio;
	if (lr > 5.0)
		ratio *= -1;
	if (lr < -5.0)
		ratio *= -1;
}

bool Outfielder::isCatch() {
	return catching;
}