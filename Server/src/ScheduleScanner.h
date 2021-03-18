#include <stdint.h>
#include "Base.h"
#include "ThreatList.h"
#include "Scanner.h"
#include <memory>

class ScheduleScanner
{
public:
	ScheduleScanner() = default;

	ScheduleScanner(const std::shared_ptr<Base>& base, const std::shared_ptr<ThreatList>& threats, 
		const std::u16string path = u"", uint32_t hours = 0, uint32_t minutes = 0);

	ScheduleScanner& operator=(const ScheduleScanner& other);


	void setScanTime(uint32_t hours, uint32_t minutes);
	void setScanPath(const std::u16string& path);

	void start();

	~ScheduleScanner() = default;

private:
	void timeMonitoring();

private:
	Scanner scanner;
	uint32_t hours = 0, minutes = 0;
	std::u16string scanPath;
};