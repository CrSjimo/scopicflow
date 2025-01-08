#include "TrackListViewModel.h"

#include <algorithm>

#include <ScopicFlow/private/SelectableViewModelManipulatorInterface_p.h>
#include <ScopicFlow/TrackViewModel.h>
#include <ScopicFlow/private/TrackListViewModel_p.h>

namespace sflow {

    class TrackListViewModelIndexObject : public QObject {
    public:
        explicit TrackListViewModelIndexObject(int index, QObject *parent = nullptr) : QObject(parent), m_index(index) {
        }

        int index() const {
            return m_index;
        }

    private:
        int m_index;
    };

    class TrackListViewModelManipulatorInterface : public SelectableViewModelManipulatorInterface {
        Q_OBJECT
    public:
        Q_INVOKABLE explicit TrackListViewModelManipulatorInterface(QObject *viewModel, QObject *parent = nullptr) : SelectableViewModelManipulatorInterface(parent) {
            m_viewModel = static_cast<TrackListViewModel *>(viewModel);
        }
        void setSelected(QObject *item, bool selected) override {
            if (auto track = m_viewModel->trackAt(static_cast<TrackListViewModelIndexObject *>(item)->index())) {
                track->setSelected(selected);
            }
        }
        bool isSelected(QObject *item) const override {
            if (auto track = m_viewModel->trackAt(static_cast<TrackListViewModelIndexObject *>(item)->index())) {
                return track->selected();
            }
            return false;
        }
        QObject *nextItem(QObject *item) const override {
            auto nextIndex = static_cast<TrackListViewModelIndexObject *>(item)->index() + 1;
            return m_viewModel->indexObjectAt(nextIndex);
        }
        QObject *previousItem(QObject *item) const override {
            auto previousIndex = static_cast<TrackListViewModelIndexObject *>(item)->index() - 1;
            return m_viewModel->indexObjectAt(previousIndex);
        }
        QObject *firstItem() const override {
            return m_viewModel->indexObjectAt(0);
        }
        QObject *lastItem() const override {
            return m_viewModel->indexObjectAt(m_viewModel->count() - 1);
        }
        QObject *currentItem() const override {
            return m_viewModel->indexObjectAt(m_viewModel->currentIndex());
        }
        void setCurrentItem(QObject *item) override {
            m_viewModel->setCurrentIndex(static_cast<TrackListViewModelIndexObject *>(item)->index());
        }
        QObjectList selection() const override {
            QObjectList list;
            for (int i = 0; i < m_viewModel->count(); i++) {
                auto track = m_viewModel->trackAt(i);
                if (track->selected()) {
                    list.append(m_viewModel->indexObjectAt(i));
                }
            }
            return list;
        }
        int compareOrder(QObject *item1, QObject *item2) const override {
            auto index1 = static_cast<TrackListViewModelIndexObject *>(item1)->index();
            auto index2 = static_cast<TrackListViewModelIndexObject *>(item2)->index();
            return index1 - index2;
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
    QObject *TrackListViewModel::indexObjectAt(int index) {
        Q_D(TrackListViewModel);
        if (index < 0 || index >= d->tracks.size()) {
            return nullptr;
        }
        if (index >= d->indexObjects.size()) {
            d->indexObjects.resize(index + 1);
        }
        return d->indexObjects[index] ? d->indexObjects[index] : (d->indexObjects[index] = new TrackListViewModelIndexObject(index, this));
    }
}
#include "TrackListViewModel.moc"