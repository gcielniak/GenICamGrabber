#pragma once

#include <chrono>
#include <boost/signals2/signal.hpp>
#include <boost/bind.hpp>
#include "Image.h"

template < typename SignalT >
class SignalConnector {
public:
	boost::signals2::signal<void(SignalT)> signal;
	boost::function<void(SignalT)> slot;

	SignalConnector() {
		slot = boost::bind(&SignalConnector::operator(), this, _1);
	}

	virtual void operator()(SignalT) {
	}

	virtual void Connect(SignalConnector& connector) {
		signal.connect(connector.slot);
	}
};

class ImageSignalConnector : public SignalConnector<const Image&> {
};
