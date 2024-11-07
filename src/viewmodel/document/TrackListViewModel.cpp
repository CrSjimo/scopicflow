#include "TrackListViewModel.h"

#include <algorithm>

namespace sflow {
    TrackListViewModel::TrackListViewModel(QObject *parent) : QObject(parent), m_currentIndex(0), m_intermediate(false), m_viewportOffset(0) {
    }
    TrackListViewModel::~TrackListViewModel() = default;
    int TrackListViewModel::currentIndex() const {
        return m_currentIndex;
    }
    void TrackListViewModel::setCurrentIndex(int index) {
        if (m_currentIndex != index) {
            m_currentIndex = index;
            emit currentIndexChanged(index);
        }
    }
    int TrackListViewModel::count() const {
        return m_tracks.size();
    }
    bool TrackListViewModel::intermediate() const {
        return m_intermediate;
    }
    void TrackListViewModel::setIntermediate(bool intermediate) {
        if (m_intermediate != intermediate) {
            m_intermediate = intermediate;
            emit intermediateChanged(intermediate);
        }
    }
    double TrackListViewModel::viewportOffset() const {
        return m_viewportOffset;
    }
    void TrackListViewModel::setViewportOffset(double offset) {
        if (m_viewportOffset != offset) {
            m_viewportOffset = offset;
            emit viewportOffsetChanged(offset);
        }
    }
    void TrackListViewModel::insertTracks(int index, const QList<TrackViewModel *> &tracks) {
        m_tracks.insert(index, tracks.size(), nullptr);
        std::copy(tracks.begin(), tracks.end(), m_tracks.begin() + index);
        if (!tracks.empty()) {
            emit tracksInserted(index, tracks);
            emit countChanged(count());
        }
    }
    void TrackListViewModel::removeTracks(int index, int count) {
        m_tracks.remove(index, count);
        if (count > 0) {
            emit tracksRemoved(index, count);
            emit countChanged(this->count());
        }
        if (m_currentIndex >= this->count()) {
            setCurrentIndex(this->count() - 1);
        }
    }
    void TrackListViewModel::rotateTracks(int index, int count, int middleIndex) {
        std::rotate(std::next(m_tracks.begin(), index), std::next(m_tracks.begin(), middleIndex), std::next(m_tracks.begin(), index + count));
        emit tracksRotated(index, middleIndex, count);
        if (m_currentIndex >= index && m_currentIndex < middleIndex) {
            setCurrentIndex(index + count - middleIndex + m_currentIndex);
        } else if (m_currentIndex >= middleIndex && m_currentIndex < index + count) {
            setCurrentIndex(m_currentIndex - index);
        }
    }
    TrackViewModel *TrackListViewModel::trackAt(int index) const {
        return m_tracks.at(index);
    }
}