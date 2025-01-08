#include "ClavierHelper_p.h"

#include <SVSCraftCore/musicpitch.h>

namespace sflow {

    namespace {
        auto _ = qmlRegisterSingletonInstance<ClavierHelper>("dev.sjimo.ScopicFlow.Private.Internal", 1, 0, "ClavierHelper", new ClavierHelper);
    }

    QString ClavierHelper::keyNameImpl(int key, int accidentalType) const {
        Q_UNUSED(this);
        SVS::MusicPitch musicPitch(key);
        return musicPitch.toString(static_cast<SVS::MusicPitch::Accidental>(accidentalType));
    }

}