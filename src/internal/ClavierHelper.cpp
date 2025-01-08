#include "ClavierHelper_p.h"

#include <SVSCraftCore/musicpitch.h>

namespace sflow {

    QString ClavierHelper::keyNameImpl(int key, int accidentalType) const {
        Q_UNUSED(this);
        SVS::MusicPitch musicPitch(key);
        return musicPitch.toString(static_cast<SVS::MusicPitch::Accidental>(accidentalType));
    }

}