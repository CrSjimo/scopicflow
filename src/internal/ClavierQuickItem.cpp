#include "ClavierQuickItem_p.h"
#include "ClavierQuickItem_p_p.h"

#include <SVSCraftCore/musicpitch.h>

#include <ScopicFlow/ClavierViewModel.h>

namespace sflow {

    namespace {
        auto _ = qmlRegisterType<ClavierQuickItem>("dev.sjimo.ScopicFlow.Private.Internal", 1, 0, "Clavier");
    }

    ClavierQuickItem::ClavierQuickItem(QQuickItem *parent) : QQuickItem(parent), d_ptr(new ClavierQuickItemPrivate) {
        Q_D(ClavierQuickItem);
        d->q_ptr = this;
    }
    ClavierQuickItem::~ClavierQuickItem() = default;

    QString ClavierQuickItem::keyNameImpl(int key, int accidentalType) const {
        Q_D(const ClavierQuickItem);
        SVS::MusicPitch musicPitch(key);
        return musicPitch.toString(static_cast<SVS::MusicPitch::Accidental>(accidentalType));
    }

}