#pragma once
#include "AWidget.hpp"
#include "nibbler.hpp"

class Core;

class WidgetOption : public AWidget {
public:
	WidgetOption(Core &);
	~WidgetOption(void);


	void	render(void);

private:
	unsigned int		_mapSize;
	char				_nameBuffer[NAME_BUFFER];

	WidgetOption &operator=(const WidgetOption&);
	WidgetOption(const WidgetOption&);
};