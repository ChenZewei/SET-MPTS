#ifndef EVENT_H
#define EVENT_H

template<typename Object,typename Time>
class Event
{
	private:
		Object *object;
		Time time;
	public:
		Object* object() const;
		const &Time time() const;
		bool operator<(const Event<Object,Time> &that) const;
		bool operator>(const Event<Object,Time> &that) const;
};

#endif
