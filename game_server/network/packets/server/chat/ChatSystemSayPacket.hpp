/// @author    Chnossos
/// @date      Created on 2026-03-31

#pragma once

// Project includes
#include "../../../../game/skill/SkillUid.hpp"

#include <l2cpp/network/Packet.hpp>

namespace details
{
    enum class SystemMessageArgumentType
    {
        Text         = 0,
        Number       = 1,
        NpcName      = 2,
        ItemName     = 3,
        SkillName    = 4,
    };

    struct SystemMessageArgument
    {
        SystemMessageArgumentType const type;

        explicit SystemMessageArgument(SystemMessageArgumentType const t) noexcept: type(t) {}
        virtual ~SystemMessageArgument() = default;

        virtual void serialize(l2cpp::Network::Packet &) = 0;
    };

    template<SystemMessageArgumentType t, typename Value>
    struct SystemMessageArgumentImpl final : public SystemMessageArgument
    {
        Value value;

        explicit SystemMessageArgumentImpl(auto &&... args)
            : SystemMessageArgument(t), value(std::forward<decltype(args)>(args)...)
        {}

        void serialize(l2cpp::Network::Packet & p) override { p << type << value; }
    };
}

#define DECLARE_SYS_MSG_ARG_TYPE(Type, ValueType)                                                             \
    template struct details::SystemMessageArgumentImpl<details::SystemMessageArgumentType::Type, ValueType>;  \
    namespace SysMsgArg {                                                                                     \
        using Type = details::SystemMessageArgumentImpl<details::SystemMessageArgumentType::Type, ValueType>; \
    }

DECLARE_SYS_MSG_ARG_TYPE(Text,      std::wstring_view);
DECLARE_SYS_MSG_ARG_TYPE(Number,    u32);
DECLARE_SYS_MSG_ARG_TYPE(NpcName,   u32);
DECLARE_SYS_MSG_ARG_TYPE(ItemName,  u32);
DECLARE_SYS_MSG_ARG_TYPE(SkillName, SkillUid);

#undef DECLARE_SYS_MSG_ARG_TYPE

namespace Network::Packet::Server { class ChatSystemSayPacket; }

class Network::Packet::Server::ChatSystemSayPacket final : public l2cpp::Network::Packet
{
public:
    explicit ChatSystemSayPacket(u32 messageId);

public:
    template<class T> requires std::is_base_of_v<details::SystemMessageArgument, T>
    ChatSystemSayPacket & appendArg(auto &&... args) {
        return appendArg(std::make_unique<T>(std::forward<decltype(args)>(args)...));
    }

private:
    ChatSystemSayPacket & appendArg(std::unique_ptr<details::SystemMessageArgument> arg);
    void finalizeImpl() override;

private:
    std::vector<std::unique_ptr<details::SystemMessageArgument>> _args;
};
