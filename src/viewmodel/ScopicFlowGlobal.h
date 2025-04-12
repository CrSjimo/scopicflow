#ifndef SCOPIC_FLOW_SCOPICFLOWGLOBAL_H
#define SCOPIC_FLOW_SCOPICFLOWGLOBAL_H

#ifndef SCOPIC_FLOW_EXPORT
#  ifdef SCOPIC_FLOW_STATIC
#    define SCOPIC_FLOW_EXPORT
#  else
#    ifdef SCOPICFLOWVIEWMODEL_LIBRARY
#      define SCOPIC_FLOW_EXPORT Q_DECL_EXPORT
#    else
#      define SCOPIC_FLOW_EXPORT Q_DECL_IMPORT
#    endif
#  endif
#endif

#ifndef Q_MOC_RUN
#  define SFLOW_VIEWMODEL_SIGNAL
#  define SFLOW_VIEWMODEL_RESET
#endif

#define SFLOW_VIEWMODEL_DECLARE_PROPERTY(type, name, read, write, reset) \
    type read() const; \
    void write(type const &); \
    Q_INVOKABLE SFLOW_VIEWMODEL_RESET void reset(); \
    Q_SIGNAL SFLOW_VIEWMODEL_SIGNAL void name##Changed();

#endif //SCOPIC_FLOW_SCOPICFLOWGLOBAL_H
