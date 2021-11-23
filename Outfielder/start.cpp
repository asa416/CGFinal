#include <string>

class State {
	std::string name;
public:
	virtual ~State() {}
	virtual void Draw() = 0;
};

class StartState : public State {
public:
	void Draw() final {

	}
};