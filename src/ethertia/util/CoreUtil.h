//
// Created by Dreamtowards on 2023/4/28.
//

#ifndef ETHERTIA_COREUTIL_H
#define ETHERTIA_COREUTIL_H

namespace et
{

    template<typename DstType, typename SrcType>
    inline static DstType force_cast(SrcType src) {
        return *reinterpret_cast<DstType*>(&src);
    }



}

#endif //ETHERTIA_COREUTIL_H
