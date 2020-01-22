#ifndef org_mitk_cbica_captk_interactivesegmentation_Activator_h
#define org_mitk_cbica_captk_interactivesegmentation_Activator_h

#include <ctkPluginActivator.h>

namespace mitk
{
  class PluginActivator : public QObject, public ctkPluginActivator
  {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org_mitk_gui_qt_multilabelsegmentation")
    Q_INTERFACES(ctkPluginActivator)

  public:
    void start(ctkPluginContext *context);
    void stop(ctkPluginContext *context);

    static ctkPluginContext* getContext();

  private:
    static ctkPluginContext* m_Context;
  };
}

#endif // ! org_mitk_cbica_captk_interactivesegmentation_Activator_h
