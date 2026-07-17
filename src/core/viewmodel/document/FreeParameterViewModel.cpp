#include "FreeParameterViewModel.h"

#include <algorithm>
#include <cmath>
#include <limits>

namespace sflow {

    FreeParameterViewModel::FreeParameterViewModel(QObject *parent) : QObject(parent) {
    }

    FreeParameterViewModel::~FreeParameterViewModel() = default;

    int FreeParameterViewModel::size() const {
        return m_values.size();
    }

    QVariant FreeParameterViewModel::valueAtIndex(int index) const {
        if (index < 0 || index >= m_values.size()) {
            return {};
        }
        return m_values.at(index);
    }

    QVariant FreeParameterViewModel::valueAt(double position) const {
        if (!std::isfinite(position) || position < 0.0) {
            return {};
        }

        const double scaledPosition = position / step();
        if (scaledPosition > std::numeric_limits<int>::max()) {
            return {};
        }
        const auto leftIndex = static_cast<int>(std::floor(scaledPosition));
        const double fraction = scaledPosition - leftIndex;
        const auto leftValue = valueAtIndex(leftIndex);
        if (qFuzzyIsNull(fraction)) {
            return leftValue;
        }

        const auto rightValue = valueAtIndex(leftIndex + 1);
        if (!leftValue.isValid() || !rightValue.isValid()) {
            return {};
        }
        return leftValue.toDouble() + fraction * (rightValue.toDouble() - leftValue.toDouble());
    }

    QList<QVariant> FreeParameterViewModel::slice(int index, int length) const {
        if (index < 0 || length < 0 || index >= m_values.size()) {
            return {};
        }
        return m_values.sliced(index, qMin(length, m_values.size() - index));
    }

    bool FreeParameterViewModel::normalizeValues(const QList<QVariant> &source, QList<QVariant> &target) {
        target.clear();
        target.reserve(source.size());
        for (const auto &value : source) {
            if (!value.isValid() || value.isNull()) {
                target.append(QVariant());
                continue;
            }
            bool ok = false;
            const double number = value.toDouble(&ok);
            if (!ok || !std::isfinite(number)) {
                return false;
            }
            target.append(number);
        }
        return true;
    }

    bool FreeParameterViewModel::splice(int index, int removeCount, const QList<QVariant> &values) {
        if (index < 0 || removeCount < 0 || index > m_values.size() || removeCount > m_values.size() - index) {
            return false;
        }

        QList<QVariant> normalizedValues;
        if (!normalizeValues(values, normalizedValues)) {
            return false;
        }
        if (removeCount == 0 && normalizedValues.isEmpty()) {
            return true;
        }

        const int oldSize = m_values.size();
        Q_EMIT aboutToSplice(index, removeCount, normalizedValues);
        QList<QVariant> updatedValues;
        updatedValues.reserve(oldSize - removeCount + normalizedValues.size());
        updatedValues.append(m_values.cbegin(), m_values.cbegin() + index);
        updatedValues.append(normalizedValues);
        updatedValues.append(m_values.cbegin() + index + removeCount, m_values.cend());
        m_values.swap(updatedValues);
        Q_EMIT spliced(index, removeCount, normalizedValues);
        Q_EMIT valuesChanged(index, removeCount, normalizedValues.size());
        if (oldSize != m_values.size()) {
            Q_EMIT sizeChanged(m_values.size());
        }
        return true;
    }

    bool FreeParameterViewModel::setValues(int index, const QList<QVariant> &values) {
        if (index < 0) {
            return false;
        }
        QList<QVariant> normalizedValues;
        if (!normalizeValues(values, normalizedValues)) {
            return false;
        }
        if (normalizedValues.isEmpty()) {
            return true;
        }

        if (index > m_values.size()) {
            QList<QVariant> extendedValues(index - m_values.size(), QVariant());
            extendedValues.append(normalizedValues);
            return splice(m_values.size(), 0, extendedValues);
        }

        const int removeCount = qMin(normalizedValues.size(), m_values.size() - index);
        return splice(index, removeCount, normalizedValues);
    }

}

#include "moc_FreeParameterViewModel.cpp"
