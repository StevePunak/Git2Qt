#ifndef TRACKINGDETAILS_H
#define TRACKINGDETAILS_H
#include <git2qt/commit.h>

namespace GIT {

class TrackingDetails
{
public:
    TrackingDetails() {}
    TrackingDetails(const Commit& local, const Commit& upstream, int aheadBy, int behindBy) :
        _local(local), _upstream(upstream), _aheadBy(aheadBy), _behindBy(behindBy), _valid(true) {}

    Commit local() const { return _local; }
    Commit upstream() const { return _upstream; }
    int aheadBy() const { return _aheadBy; }
    int behindBy() const { return _behindBy; }

    bool isValid() const { return _valid; }

private:
    Commit _local;
    Commit _upstream;
    int _aheadBy = 0;
    int _behindBy = 0;
    bool _valid = false;
};

} // namespace GIT

#endif // TRACKINGDETAILS_H
