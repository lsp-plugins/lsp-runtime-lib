/*
 * ITask.cpp
 *
 *  Created on: 27 янв. 2016 г.
 *      Author: sadko
 */

#include <lsp-plug.in/common/types.h>
#include <lsp-plug.in/ipc/ITask.h>

namespace lsp
{
    namespace ipc
    {
        ITask::ITask()
        {
            nState  = TS_IDLE;
            nCode   = 0;
            pNext   = NULL;
        }

        ITask::~ITask()
        {
        }

        status_t ITask::run()
        {
            return 0;
        }
    }

} /* namespace lsp */
