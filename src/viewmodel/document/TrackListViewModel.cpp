#include "TrackListViewModel.h"

#include <algorithm>

#include <ScopicFlow/private/SelectableViewModelManipulatorInterface_p.h>
#include <ScopicFlow/TrackViewModel.h>
#include <ScopicFlow/private/TrackListViewModel_p.h>

namespace sflow {

    class TrackListViewModelManipulatorInterface : public SelectableViewModelManipulatorInterface {
        Q_OBJECT
    public:
        Q_INVOKABLE explicit TrackListViewModelManipulatorInterface(QObject *viewModel, QObject *parent = nullptr) : SelectableViewModelManipulatorInterface(parent) {
            m_viewModel = static_cast<TrackListViewModel *>(viewModel);
        }
        void setSelected(const QVariant &item, bool selected) override {
            if (auto track = m_viewModel->trackAt(item.toInt())) {
                track->setSelected(selected);
            }
        }
        bool isSelected(const QVariant &item) const override {
            if (auto track = m_viewModel->trackAt(item.toInt())) {
                return track->selected();
            }
            return false;
        }
        QVariant nextItem(const QVariant &item) const override {
            return item.toInt() + 1;
        }
        QVariant previousItem(const QVariant &item) const override {
            return item.toInt() - 1;
        }
        QVariant firstItem() const override {
            return 0;
        }
        QVariant lastItem() const override {
            return m_viewModel->count() - 1;
        }
        QVariant currentItem() const override {
            return m_viewModel->currentIndex();
        }
        void setCurrentItem(const QVariant &item) override {
            m_viewModel->setCurrentIndex(item.toInt());
        }
        QVariantList selection() const override {
            QVariantList list;
            for (int i = 0; i < m_viewModel->count(); i++) {
                auto track = m_viewModel->trackAt(i);
                if (track->selected()) {
                    list.append(i);
                }
            }
            return list;
        }
        int compareOrder(const QVariant &item1, const QVariant &item2) const override {
            auto index1 = item1.toInt();
            auto index2 = item2.toInt();
            return index1 - index2;
        }
        bool isValidItem(const QVariant &item) const override {
            auto index = item.toInt();
            return index >= 0 && index < m_viewModel->count();
        }
        qsizetype getId(const QVariant &item) const override {
            return item.toInt();
        }
        QVariant fromId(qsizetype id) const override {
            return static_cast<int>(id);
        }
        QObject *viewModel() const override {
            return m_viewModel;
        }

    private:
        TrackListViewModel *m_viewModel;
    };

    namespace {
        struct _ {
            _() {
                SelectableViewModelManipulatorInterface::registerViewModelInterface(TrackListViewModel::staticMetaObject.className(), &TrackListViewModelManipulatorInterface::staticMetaObject);
            }
        } _;
    }

    TrackListViewModel::TrackListViewModel(QObject *parent) : QObject(parent), d_ptr(new TrackListViewModelPrivate) {
        Q_D(TrackListViewModel);
        d->q_ptr = this;
    }
    TrackListViewModel::~TrackListViewModel() = default;
    int TrackListViewModel::currentIndex() const {
        Q_D(const TrackListViewModel);
        return d->currentIndex;
    }
    void TrackListViewModel::setCurrentIndex(int index) {
        Q_D(TrackListViewModel);
        if (d->currentIndex != index) {
            d->currentIndex = index;
            emit currentIndexChanged(index);
        }
    }
    int TrackListViewModel::count() const {
        Q_D(const TrackListViewModel);
        return d->tracks.size();
    }
    bool TrackListViewModel::intermediate() const {
        Q_D(const TrackListViewModel);
        return d->intermediate;
    }
    void TrackListViewModel::setIntermediate(bool intermediate) {
        Q_D(TrackListViewModel);
        if (d->intermediate != intermediate) {
            d->intermediate = intermediate;
            emit intermediateChanged(intermediate);
        }
    }
    double TrackListViewModel::viewportOffset() const {
        Q_D(const TrackListViewModel);
        return d->viewportOffset;
    }
    void TrackListViewModel::setViewportOffset(double offset) {
        Q_D(TrackListViewModel);
        if (d->viewportOffset != offset) {
            d->viewportOffset = offset;
            emit viewportOffsetChanged(offset);
        }
    }
    void TrackListViewModel::insertTracks(int index, const QList<TrackViewModel *> &tracks) {
        Q_D(TrackListViewModel);
        d->tracks.insert(index, tracks.size(), nullptr);
        std::copy(tracks.begin(), tracks.end(), d->tracks.begin() + index);
        for (auto track : tracks) {
            track->setParent(this);
        }
        if (!tracks.empty()) {
            emit tracksInserted(index, tracks);
            emit countChanged(count());
        }
    }
    void TrackListViewModel::removeTracks(int index, int count) {
        Q_D(TrackListViewModel);
        std::for_each_n(d->tracks.begin(), count, [](TrackViewModel *track) {
            delete track;
        });
        d->tracks.remove(index, count);
        if (count > 0) {
            emit tracksRemoved(index, count);
            emit countChanged(this->count());
        }
        if (d->currentIndex >= this->count()) {
            setCurrentIndex(this->count() - 1);
        }
    }
    void TrackListViewModel::rotateTracks(int index, int count, int middleIndex) {
        Q_D(TrackListViewModel);
        std::rotate(std::next(d->tracks.begin(), index), std::next(d->tracks.begin(), middleIndex), std::next(d->tracks.begin(), index + count));
        emit tracksRotated(index, middleIndex, count);
        if (d->currentIndex >= index && d->currentIndex < middleIndex) {
            setCurrentIndex(index + count - middleIndex + d->currentIndex);
        } else if (d->currentIndex >= middleIndex && d->currentIndex < index + count) {
            setCurrentIndex(d->currentIndex - index);
        }
    }
    TrackViewModel *TrackListViewModel::trackAt(int index) const {
        Q_D(const TrackListViewModel);
        return d->tracks.at(index);
    }
}
#include "TrackListViewModel.moc"