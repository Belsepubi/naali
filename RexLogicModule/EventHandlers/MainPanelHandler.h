// For conditions of distribution and use, see copyright notice in license.txt

#ifndef incl_RexLogic_MainPanelHandler_h
#define incl_RexLogic_MainPanelHandler_h

#include "Foundation.h"
#include "RexLogicModule.h"

#include <QObject>

namespace RexLogic
{
    class MainPanelHandler : public QObject
    {

    Q_OBJECT

    public:
        MainPanelHandler(Foundation::Framework *framework, RexLogicModule *logic_module);
        ~MainPanelHandler();

        void ConnectToLoginHandler();

    public slots:
        void LogoutRequested();
        void QuitRequested();

    private:
        Foundation::Framework *framework_;
        RexLogicModule *rex_logic_module_;

    };
}

#endif // incl_RexLogic_MainPanelHandler_h