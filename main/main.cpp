#include <vmchset.h>
#include <vmsys.h>
#include <vmsrvmng.h>
#include <vmtimer.h>

#define IBase GS_IBase

#include "gs_id.h"
#include "srv_ucm_interface.h"
#include <cstring>

static S32 dial_callback(gs_srv_event_struct *param)
{
    return 0;
}

void timer1(int a)
{
    vm_delete_timer_ex(a);
    vm_exit_app();
}

static void handle_system_event(VMINT message, VMINT param)
{
    if (message == VM_MSG_CREATE)
    {
        IUcm *ucm = NULL;

        VMINT ret = vm_create_service(SID_UCMSERVICE, IID_IUCM, (void **)&ucm);

        if (ret >= 0 && ucm)
        {
            gs_srv_ucm_dial_act_req_struct req;

            memset(&req, 0, sizeof(req));

            req.call_type = GS_SRV_UCM_VOICE_CALL_TYPE;

            req.module_id = GS_SRV_UCM_MODULE_ORIGIN_COMMON;

            req.is_ip_dial = 0;

            vm_ascii_to_ucs2((VMWSTR)req.num_uri, sizeof(req.num_uri), "123456789");

            gs_srv_ucm_result_enum permit = IUcm_query_act_permit(ucm, GS_SRV_UCM_DIAL_ACT, &req);

            if (permit == GS_SRV_UCM_RESULT_OK)
            {
                IUcm_act_request(ucm, GS_SRV_UCM_DIAL_ACT, &req, NULL, dial_callback);
            }

            IUcm_release(ucm);
        }

        vm_create_timer_ex(3000, timer1);
    }
}

void vm_main(void)
{
    vm_reg_sysevt_callback(handle_system_event);
}
