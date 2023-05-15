#ifndef __OBSERVER_PATTERN_H__
#define __OBSERVER_PATTERN_H__

class ISubscriber
{
public:
	virtual ~ISubscriber() = default;
	virtual void UpdateSubscriber(int id, bool state) = 0;
};

class IPublisher
{
public:
	virtual ~IPublisher() = default;
	virtual void Attach(ISubscriber* Subscriber, int id) = 0;
	virtual void Detach(ISubscriber* Subscriber, int id) = 0;
	virtual void Notify(int id, bool state) = 0;
};

class Publisher : public IPublisher
{
public:
	~Publisher() override = default;

	void Attach(ISubscriber* sub, int id) override
	{
		subscribers[id].emplace_back(sub);
	}

	void Detach(ISubscriber* sub, int id) override
	{
		std::erase_if(subscribers[id], [sub](ISubscriber* elem) { return sub == elem; });
	}

	void Notify(int id, bool state) override
	{
		std::ranges::for_each(
			subscribers[id],
			[id, state](ISubscriber* sub)
			{
				sub->UpdateSubscriber(id, state);
			}
		);
	}

	void CleanUpNullSubscribers()
	{
		for (auto& [id, subVector] : subscribers)
		{
			std::erase_if
			(
				subVector,
				[](ISubscriber* sub)
				{
					return !sub;
				}
			);
		}
	}

private:
	std::unordered_map<int, std::vector<ISubscriber*>> subscribers;
};

class Subscriber : public ISubscriber
{
public:
	explicit Subscriber(Publisher& pub)
		: publisher(pub) {}

	void UpdateSubscriber(int id, bool status) override
	{}

	void UnsubscribeFromList()
	{
		for (auto const& element : trackingIDs)
		{
			publisher.Detach(this, element);
		}
	}

protected:
	virtual void AttachToGlobalSwitches() = 0;
	void AttachToPublisherID(int id)
	{
		bool alreadyTrackingID = std::ranges::any_of(trackingIDs, [id](int i) { return i == id; });
		if (!alreadyTrackingID)
		{
			trackingIDs.emplace_back(id);
			publisher.Attach(this, id);
		}
	}

private:
	Publisher& publisher;
	std::vector<int> trackingIDs;
};

#endif	// __OBSERVER_PATTERN_H__