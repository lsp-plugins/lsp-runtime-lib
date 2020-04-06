/*
 * IExecutor.cpp
 *
 *  Created on: 27 янв. 2016 г.
 *      Author: sadko
 */

#include <lsp-plug.in/common/types.h>
#include <lsp-plug.in/runtime/ipc/IExecutor.h>

namespace lsp
{
    namespace ipc
    {
        IExecutor::IExecutor()
        {
        }

        IExecutor::~IExecutor()
        {
        }

        bool IExecutor::submit(ITask *task)
        {
            return false;
        }

        void IExecutor::shutdown()
        {
        }
    } /* namespace lsp */
} /* namespace lsp */
