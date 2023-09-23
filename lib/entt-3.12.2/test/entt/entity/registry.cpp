#include <cstddef>
#include <functional>
#include <iterator>
#include <memory>
#include <tuple>
#include <type_traits>
#include <unordered_set>
#include <utility>
#include <gtest/gtest.h>
#include <entt/core/hashed_string.hpp>
#include <entt/core/type_info.hpp>
#include <entt/entity/entity.hpp>
#include <entt/entity/registry.hpp>
#include "../common/config.h"

struct empty_type {};

struct no_eto_type {
    static constexpr std::size_t page_size = 1024u;
};

bool operator==(const no_eto_type &lhs, const no_eto_type &rhs) {
    return &lhs == &rhs;
}

struct stable_type {
    static constexpr auto in_place_delete = true;
    int value;
};

struct non_default_constructible {
    non_default_constructible(int v)
        : value{v} {}

    int value;
};

struct aggregate {
    int value{};
};

struct listener {
    template<typename Type>
    static void sort(entt::registry &registry) {
        registry.sort<Type>([](auto lhs, auto rhs) { return lhs < rhs; });
    }

    void incr(const entt::registry &, entt::entity entity) {
        last = entity;
        ++counter;
    }

    void decr(const entt::registry &, entt::entity entity) {
        last = entity;
        --counter;
    }

    entt::entity last{entt::null};
    int counter{0};
};

struct owner {
    void receive(const entt::registry &ref) {
        parent = &ref;
    }

    const entt::registry *parent{nullptr};
};

struct destruction_order {
    using ctx_check_type = int;

    destruction_order(const entt::registry &ref, bool &ctx)
        : registry{&ref},
          ctx_check{&ctx} {
        *ctx_check = (registry->ctx().find<ctx_check_type>() != nullptr);
    }

    ~destruction_order() {
        *ctx_check = *ctx_check && (registry->ctx().find<ctx_check_type>() != nullptr);
    }

private:
    const entt::registry *registry;
    bool *ctx_check{};
};

enum class small_entity : std::uint32_t {};

struct small_entity_traits {
    using value_type = small_entity;
    using entity_type = uint32_t;
    using version_type = uint16_t;
    static constexpr entity_type entity_mask = 0xFF;
    static constexpr entity_type version_mask = 0x00;
};

template<>
struct entt::entt_traits<small_entity>: entt::basic_entt_traits<small_entity_traits> {
    using base_type = entt::basic_entt_traits<small_entity_traits>;
    static constexpr auto page_size = ENTT_SPARSE_PAGE;
};

TEST(Registry, Context) {
    entt::registry registry;
    auto &ctx = registry.ctx();
    const auto &cctx = std::as_const(registry).ctx();

    ASSERT_FALSE(ctx.contains<char>());
    ASSERT_FALSE(cctx.contains<const int>());
    ASSERT_EQ(ctx.find<char>(), nullptr);
    ASSERT_EQ(cctx.find<const int>(), nullptr);

    ctx.emplace<char>();
    ctx.emplace<int>();

    ASSERT_TRUE(ctx.contains<char>());
    ASSERT_TRUE(cctx.contains<int>());
    ASSERT_NE(ctx.find<const char>(), nullptr);
    ASSERT_NE(cctx.find<const int>(), nullptr);

    ASSERT_FALSE(ctx.erase<double>());
    ASSERT_TRUE(ctx.erase<int>());

    ASSERT_TRUE(ctx.contains<const char>());
    ASSERT_FALSE(cctx.contains<const int>());
    ASSERT_NE(ctx.find<char>(), nullptr);
    ASSERT_EQ(cctx.find<int>(), nullptr);

    ASSERT_FALSE(ctx.erase<int>());
    ASSERT_TRUE(ctx.erase<char>());

    ctx.emplace<char>('c');
    ctx.emplace<int>(42);

    ASSERT_EQ(ctx.emplace<char>('a'), 'c');
    ASSERT_EQ(ctx.find<const char>(), cctx.find<char>());
    ASSERT_EQ(ctx.get<char>(), cctx.get<const char>());
    ASSERT_EQ(ctx.get<char>(), 'c');

    ASSERT_EQ(ctx.emplace<const int>(0), 42);
    ASSERT_EQ(ctx.find<const int>(), cctx.find<int>());
    ASSERT_EQ(ctx.get<int>(), cctx.get<const int>());
    ASSERT_EQ(ctx.get<int>(), 42);

    ASSERT_EQ(ctx.find<double>(), nullptr);
    ASSERT_EQ(cctx.find<double>(), nullptr);

    ASSERT_EQ(ctx.insert_or_assign<char>('a'), 'a');
    ASSERT_EQ(ctx.find<const char>(), cctx.find<char>());
    ASSERT_EQ(ctx.get<char>(), cctx.get<const char>());
    ASSERT_EQ(ctx.get<const char>(), 'a');

    ASSERT_EQ(ctx.insert_or_assign<const int>(0), 0);
    ASSERT_EQ(ctx.find<const int>(), cctx.find<int>());
    ASSERT_EQ(ctx.get<int>(), cctx.get<const int>());
    ASSERT_EQ(ctx.get<int>(), 0);
}

TEST(Registry, ContextHint) {
    using namespace entt::literals;

    entt::registry registry;
    auto &ctx = registry.ctx();
    const auto &cctx = std::as_const(registry).ctx();

    ctx.emplace<int>(42);
    ctx.emplace_as<int>("other"_hs, 3);

    ASSERT_TRUE(ctx.contains<int>());
    ASSERT_TRUE(cctx.contains<const int>("other"_hs));
    ASSERT_FALSE(ctx.contains<char>("other"_hs));

    ASSERT_NE(cctx.find<const int>(), nullptr);
    ASSERT_NE(ctx.find<int>("other"_hs), nullptr);
    ASSERT_EQ(cctx.find<const char>("other"_hs), nullptr);

    ASSERT_EQ(ctx.get<int>(), 42);
    ASSERT_EQ(cctx.get<const int>("other"_hs), 3);

    ctx.insert_or_assign(3);
    ctx.insert_or_assign("other"_hs, 42);

    ASSERT_EQ(ctx.get<const int>(), 3);
    ASSERT_EQ(cctx.get<int>("other"_hs), 42);

    ASSERT_FALSE(ctx.erase<char>("other"_hs));
    ASSERT_TRUE(ctx.erase<int>());

    ASSERT_TRUE(cctx.contains<int>("other"_hs));
    ASSERT_EQ(ctx.get<int>("other"_hs), 42);

    ASSERT_TRUE(ctx.erase<int>("other"_hs));

    ASSERT_FALSE(cctx.contains<int>("other"_hs));
    ASSERT_EQ(ctx.find<int>("other"_hs), nullptr);
}

TEST(Registry, ContextAsRef) {
    entt::registry registry;
    int value{3};

    registry.ctx().emplace<int &>(value);

    ASSERT_NE(registry.ctx().find<int>(), nullptr);
    ASSERT_NE(registry.ctx().find<const int>(), nullptr);
    ASSERT_NE(std::as_const(registry).ctx().find<const int>(), nullptr);
    ASSERT_EQ(registry.ctx().get<const int>(), 3);
    ASSERT_EQ(registry.ctx().get<int>(), 3);

    registry.ctx().get<int>() = 42;

    ASSERT_EQ(registry.ctx().get<int>(), 42);
    ASSERT_EQ(value, 42);

    value = 3;

    ASSERT_EQ(std::as_const(registry).ctx().get<const int>(), 3);
}

TEST(Registry, ContextAsConstRef) {
    entt::registry registry;
    int value{3};

    registry.ctx().emplace<const int &>(value);

    ASSERT_EQ(registry.ctx().find<int>(), nullptr);
    ASSERT_NE(registry.ctx().find<const int>(), nullptr);
    ASSERT_NE(std::as_const(registry).ctx().find<const int>(), nullptr);
    ASSERT_EQ(registry.ctx().get<const int>(), 3);

    value = 42;

    ASSERT_EQ(std::as_const(registry).ctx().get<const int>(), 42);
}

TEST(Registry, Functionalities) {
    using traits_type = entt::entt_traits<entt::entity>;

    entt::registry registry;

    ASSERT_NO_FATAL_FAILURE([[maybe_unused]] auto alloc = registry.get_allocator());

    ASSERT_EQ(registry.size(), 0u);
    ASSERT_EQ(registry.alive(), 0u);
    ASSERT_NO_FATAL_FAILURE(registry.reserve(42));
    ASSERT_EQ(registry.capacity(), 42u);
    ASSERT_TRUE(registry.empty());

    ASSERT_EQ(registry.storage<int>().size(), 0u);
    ASSERT_EQ(registry.storage<char>().size(), 0u);
    ASSERT_TRUE(registry.storage<int>().empty());
    ASSERT_TRUE(registry.storage<char>().empty());

    const auto e0 = registry.create();
    const auto e1 = registry.create();

    registry.emplace<int>(e1);
    registry.emplace<char>(e1);

    ASSERT_TRUE(registry.all_of<>(e0));
    ASSERT_FALSE(registry.any_of<>(e1));

    ASSERT_EQ(registry.storage<int>().size(), 1u);
    ASSERT_EQ(registry.storage<char>().size(), 1u);
    ASSERT_FALSE(registry.storage<int>().empty());
    ASSERT_FALSE(registry.storage<char>().empty());

    ASSERT_NE(e0, e1);

    ASSERT_FALSE((registry.all_of<int, const char>(e0)));
    ASSERT_TRUE((registry.all_of<const int, char>(e1)));
    ASSERT_FALSE((registry.any_of<int, const double>(e0)));
    ASSERT_TRUE((registry.any_of<const int, double>(e1)));

    ASSERT_EQ(registry.try_get<int>(e0), nullptr);
    ASSERT_NE(registry.try_get<int>(e1), nullptr);
    ASSERT_EQ(registry.try_get<char>(e0), nullptr);
    ASSERT_NE(registry.try_get<char>(e1), nullptr);
    ASSERT_EQ(registry.try_get<double>(e0), nullptr);
    ASSERT_EQ(registry.try_get<double>(e1), nullptr);

    ASSERT_EQ(registry.emplace<int>(e0, 42), 42);
    ASSERT_EQ(registry.emplace<char>(e0, 'c'), 'c');
    ASSERT_NO_FATAL_FAILURE(registry.erase<int>(e1));
    ASSERT_NO_FATAL_FAILURE(registry.erase<char>(e1));

    ASSERT_TRUE((registry.all_of<const int, char>(e0)));
    ASSERT_FALSE((registry.all_of<int, const char>(e1)));
    ASSERT_TRUE((registry.any_of<const int, double>(e0)));
    ASSERT_FALSE((registry.any_of<int, const double>(e1)));

    const auto e2 = registry.create();

    registry.emplace_or_replace<int>(e2, registry.get<int>(e0));
    registry.emplace_or_replace<char>(e2, registry.get<char>(e0));

    ASSERT_TRUE((registry.all_of<int, char>(e2)));
    ASSERT_EQ(registry.get<int>(e0), 42);
    ASSERT_EQ(registry.get<char>(e0), 'c');

    ASSERT_NE(registry.try_get<int>(e0), nullptr);
    ASSERT_NE(registry.try_get<char>(e0), nullptr);
    ASSERT_EQ(registry.try_get<double>(e0), nullptr);
    ASSERT_EQ(*registry.try_get<int>(e0), 42);
    ASSERT_EQ(*registry.try_get<char>(e0), 'c');

    ASSERT_EQ(std::get<0>(registry.get<int, char>(e0)), 42);
    ASSERT_EQ(*std::get<0>(registry.try_get<int, char, double>(e0)), 42);
    ASSERT_EQ(std::get<1>(static_cast<const entt::registry &>(registry).get<int, char>(e0)), 'c');
    ASSERT_EQ(*std::get<1>(static_cast<const entt::registry &>(registry).try_get<int, char, double>(e0)), 'c');

    ASSERT_EQ(registry.get<int>(e0), registry.get<int>(e2));
    ASSERT_EQ(registry.get<char>(e0), registry.get<char>(e2));
    ASSERT_NE(&registry.get<int>(e0), &registry.get<int>(e2));
    ASSERT_NE(&registry.get<char>(e0), &registry.get<char>(e2));

    ASSERT_EQ(registry.patch<int>(e0, [](auto &instance) { instance = 2; }), 2);
    ASSERT_EQ(registry.replace<int>(e0, 3), 3);

    ASSERT_NO_FATAL_FAILURE(registry.emplace_or_replace<int>(e0, 1));
    ASSERT_NO_FATAL_FAILURE(registry.emplace_or_replace<int>(e1, 1));
    ASSERT_EQ(static_cast<const entt::registry &>(registry).get<int>(e0), 1);
    ASSERT_EQ(static_cast<const entt::registry &>(registry).get<int>(e1), 1);

    ASSERT_EQ(registry.size(), 3u);
    ASSERT_EQ(registry.alive(), 3u);
    ASSERT_FALSE(registry.empty());

    ASSERT_EQ(traits_type::to_version(e2), 0u);
    ASSERT_EQ(registry.current(e2), 0u);
    ASSERT_NO_FATAL_FAILURE(registry.destroy(e2));
    ASSERT_EQ(traits_type::to_version(e2), 0u);
    ASSERT_EQ(registry.current(e2), 1u);

    ASSERT_TRUE(registry.valid(e0));
    ASSERT_TRUE(registry.valid(e1));
    ASSERT_FALSE(registry.valid(e2));

    ASSERT_EQ(registry.size(), 3u);
    ASSERT_EQ(registry.alive(), 2u);
    ASSERT_FALSE(registry.empty());

    ASSERT_NO_FATAL_FAILURE(registry.clear());

    ASSERT_EQ(registry.size(), 3u);
    ASSERT_EQ(registry.alive(), 0u);
    ASSERT_TRUE(registry.empty());

    const auto e3 = registry.create();

    ASSERT_EQ(registry.get_or_emplace<int>(e3, 3), 3);
    ASSERT_EQ(registry.get_or_emplace<char>(e3, 'c'), 'c');

    ASSERT_EQ(registry.storage<int>().size(), 1u);
    ASSERT_EQ(registry.storage<char>().size(), 1u);
    ASSERT_FALSE(registry.storage<int>().empty());
    ASSERT_FALSE(registry.storage<char>().empty());
    ASSERT_TRUE((registry.all_of<int, char>(e3)));
    ASSERT_EQ(registry.get<int>(e3), 3);
    ASSERT_EQ(registry.get<char>(e3), 'c');

    ASSERT_NO_FATAL_FAILURE(registry.clear<int>());

    ASSERT_EQ(registry.storage<int>().size(), 0u);
    ASSERT_EQ(registry.storage<char>().size(), 1u);
    ASSERT_TRUE(registry.storage<int>().empty());
    ASSERT_FALSE(registry.storage<char>().empty());

    ASSERT_NO_FATAL_FAILURE(registry.clear());

    ASSERT_EQ(registry.storage<int>().size(), 0u);
    ASSERT_EQ(registry.storage<char>().size(), 0u);
    ASSERT_TRUE(registry.storage<int>().empty());
    ASSERT_TRUE(registry.storage<char>().empty());

    const auto e4 = registry.create();
    const auto e5 = registry.create();

    registry.emplace<int>(e4);

    ASSERT_EQ(registry.remove<int>(e4), 1u);
    ASSERT_EQ(registry.remove<int>(e5), 0u);

    ASSERT_EQ(registry.storage<int>().size(), 0u);
    ASSERT_EQ(registry.storage<char>().size(), 0u);
    ASSERT_TRUE(registry.storage<int>().empty());
}

TEST(Registry, Constructors) {
    entt::registry registry;
    entt::registry other{42};

    ASSERT_TRUE(registry.empty());
    ASSERT_TRUE(other.empty());

    ASSERT_EQ(registry.released(), 0u);
    ASSERT_EQ(other.released(), 0u);
}

TEST(Registry, Move) {
    entt::registry registry;
    const auto entity = registry.create();
    owner test{};

    registry.on_construct<int>().connect<&owner::receive>(test);
    registry.on_destroy<int>().connect<&owner::receive>(test);

    ASSERT_EQ(test.parent, nullptr);

    registry.emplace<int>(entity);

    ASSERT_EQ(test.parent, &registry);

    entt::registry other{std::move(registry)};
    other.erase<int>(entity);

    registry = {};
    registry.emplace<int>(registry.create(entity));

    ASSERT_EQ(test.parent, &other);

    registry = std::move(other);
    registry.emplace<int>(entity);
    registry.emplace<int>(registry.create(entity));

    ASSERT_EQ(test.parent, &registry);
}

TEST(Registry, Swap) {
    entt::registry registry;
    const auto entity = registry.create();
    owner test{};

    registry.on_construct<int>().connect<&owner::receive>(test);
    registry.on_destroy<int>().connect<&owner::receive>(test);

    ASSERT_EQ(test.parent, nullptr);

    registry.emplace<int>(entity);

    ASSERT_EQ(test.parent, &registry);

    entt::registry other;
    other.swap(registry);
    other.erase<int>(entity);

    registry = {};
    registry.emplace<int>(registry.create(entity));

    ASSERT_EQ(test.parent, &other);

    registry.swap(other);
    registry.emplace<int>(entity);
    registry.emplace<int>(registry.create(entity));

    ASSERT_EQ(test.parent, &registry);
}

TEST(Registry, ReplaceAggregate) {
    entt::registry registry;
    const auto entity = registry.create();

    registry.emplace<aggregate>(entity, 0);
    auto &instance = registry.replace<aggregate>(entity, 42);

    ASSERT_EQ(instance.value, 42);
}

TEST(Registry, EmplaceOrReplaceAggregate) {
    entt::registry registry;
    const auto entity = registry.create();
    auto &instance = registry.emplace_or_replace<aggregate>(entity, 42);

    ASSERT_EQ(instance.value, 42);
}

TEST(Registry, Identifiers) {
    using traits_type = entt::entt_traits<entt::entity>;

    entt::registry registry;
    const auto pre = registry.create();

    ASSERT_EQ(traits_type::to_integral(pre), traits_type::to_entity(pre));

    registry.release(pre);
    const auto post = registry.create();

    ASSERT_NE(pre, post);
    ASSERT_EQ(traits_type::to_entity(pre), traits_type::to_entity(post));
    ASSERT_NE(traits_type::to_version(pre), traits_type::to_version(post));
    ASSERT_NE(traits_type::to_version(pre), registry.current(pre));
    ASSERT_EQ(traits_type::to_version(post), registry.current(post));

    const auto invalid = traits_type::combine(traits_type::to_entity(post) + 1u, {});

    ASSERT_EQ(traits_type::to_version(invalid), typename traits_type::version_type{});
    ASSERT_EQ(registry.current(invalid), traits_type::to_version(entt::tombstone));
}

TEST(Registry, Data) {
    using traits_type = entt::entt_traits<entt::entity>;

    entt::registry registry;

    ASSERT_EQ(std::as_const(registry).data(), nullptr);

    const auto entity = registry.create();

    ASSERT_EQ(*std::as_const(registry).data(), entity);

    const auto other = registry.create();
    registry.release(entity);

    ASSERT_EQ(*std::as_const(registry).data(), other);
    ASSERT_EQ(*(std::as_const(registry).data() + 1u), traits_type::next(entity));
}

TEST(Registry, CreateManyEntitiesAtOnce) {
    using traits_type = entt::entt_traits<entt::entity>;

    entt::registry registry;
    entt::entity entities[3];

    const auto entity = registry.create();
    registry.release(registry.create());
    registry.release(entity);
    registry.release(registry.create());

    registry.create(std::begin(entities), std::end(entities));

    ASSERT_TRUE(registry.valid(entities[0]));
    ASSERT_TRUE(registry.valid(entities[1]));
    ASSERT_TRUE(registry.valid(entities[2]));

    ASSERT_EQ(traits_type::to_entity(entities[0]), 0u);
    ASSERT_EQ(traits_type::to_version(entities[0]), 2u);

    ASSERT_EQ(traits_type::to_entity(entities[1]), 1u);
    ASSERT_EQ(traits_type::to_version(entities[1]), 1u);

    ASSERT_EQ(traits_type::to_entity(entities[2]), 2u);
    ASSERT_EQ(traits_type::to_version(entities[2]), 0u);
}

TEST(Registry, CreateManyEntitiesAtOnceWithListener) {
    entt::registry registry;
    entt::entity entities[3];
    listener listener;

    registry.on_construct<int>().connect<&listener::incr>(listener);
    registry.create(std::begin(entities), std::end(entities));
    registry.insert(std::begin(entities), std::end(entities), 42);
    registry.insert(std::begin(entities), std::end(entities), 'c');

    ASSERT_EQ(registry.get<int>(entities[0]), 42);
    ASSERT_EQ(registry.get<char>(entities[1]), 'c');
    ASSERT_EQ(listener.counter, 3);

    registry.on_construct<int>().disconnect<&listener::incr>(listener);
    registry.on_construct<empty_type>().connect<&listener::incr>(listener);
    registry.create(std::begin(entities), std::end(entities));
    registry.insert(std::begin(entities), std::end(entities), 'a');
    registry.insert<empty_type>(std::begin(entities), std::end(entities));

    ASSERT_TRUE(registry.all_of<empty_type>(entities[0]));
    ASSERT_EQ(registry.get<char>(entities[2]), 'a');
    ASSERT_EQ(listener.counter, 6);
}

TEST(Registry, CreateWithHint) {
    using traits_type = entt::entt_traits<entt::entity>;

    entt::registry registry;
    auto e3 = registry.create(entt::entity{3});
    auto e2 = registry.create(entt::entity{3});

    ASSERT_EQ(e2, entt::entity{1});
    ASSERT_FALSE(registry.valid(entt::entity{0}));
    ASSERT_FALSE(registry.valid(entt::entity{2}));
    ASSERT_EQ(e3, entt::entity{3});

    registry.release(e2);

    ASSERT_EQ(traits_type::to_version(e2), 0u);
    ASSERT_EQ(registry.current(e2), 1u);

    e2 = registry.create();
    auto e1 = registry.create(entt::entity{2});

    ASSERT_EQ(traits_type::to_entity(e2), 1u);
    ASSERT_EQ(traits_type::to_version(e2), 1u);

    ASSERT_EQ(traits_type::to_entity(e1), 2u);
    ASSERT_EQ(traits_type::to_version(e1), 0u);

    registry.release(e1);
    registry.release(e2);
    auto e0 = registry.create(entt::entity{0});

    ASSERT_EQ(e0, entt::entity{0});
    ASSERT_EQ(traits_type::to_version(e0), 0u);
}

TEST(Registry, CreateClearCycle) {
    using traits_type = entt::entt_traits<entt::entity>;

    entt::registry registry;
    entt::entity pre{}, post{};

    for(int i = 0; i < 10; ++i) {
        const auto entity = registry.create();
        registry.emplace<double>(entity);
    }

    registry.clear();

    for(int i = 0; i < 7; ++i) {
        const auto entity = registry.create();
        registry.emplace<int>(entity);

        if(i == 3) {
            pre = entity;
        }
    }

    registry.clear();

    for(int i = 0; i < 5; ++i) {
        const auto entity = registry.create();

        if(i == 3) {
            post = entity;
        }
    }

    ASSERT_FALSE(registry.valid(pre));
    ASSERT_TRUE(registry.valid(post));
    ASSERT_NE(traits_type::to_version(pre), traits_type::to_version(post));
    ASSERT_EQ(traits_type::to_version(pre) + 1, traits_type::to_version(post));
    ASSERT_EQ(registry.current(pre), registry.current(post));
}

TEST(Registry, CreateDestroyReleaseCornerCase) {
    entt::registry registry;

    const auto e0 = registry.create();
    const auto e1 = registry.create();

    registry.destroy(e0);
    registry.release(e1);

    registry.each([](auto) { FAIL(); });

    ASSERT_EQ(registry.current(e0), 1u);
    ASSERT_EQ(registry.current(e1), 1u);
}

ENTT_DEBUG_TEST(RegistryDeathTest, CreateTooManyEntities) {
    entt::basic_registry<small_entity> registry;
    std::vector<small_entity> entities(entt::entt_traits<small_entity>::to_entity(entt::null));
    registry.create(entities.begin(), entities.end());

    ASSERT_DEATH([[maybe_unused]] const auto entity = registry.create(), "");
}

TEST(Registry, DestroyVersion) {
    entt::registry registry;

    const auto e0 = registry.create();
    const auto e1 = registry.create();

    ASSERT_EQ(registry.current(e0), 0u);
    ASSERT_EQ(registry.current(e1), 0u);

    registry.destroy(e0);
    registry.destroy(e1, 3);

    ASSERT_EQ(registry.current(e0), 1u);
    ASSERT_EQ(registry.current(e1), 3u);
}

ENTT_DEBUG_TEST(RegistryDeathTest, DestroyVersion) {
    entt::registry registry;
    const auto entity = registry.create();

    registry.destroy(entity);

    ASSERT_DEATH(registry.destroy(entity), "");
    ASSERT_DEATH(registry.destroy(entity, 3), "");
}

TEST(Registry, DestroyRange) {
    entt::registry registry;
    const auto iview = registry.view<int>();
    const auto icview = registry.view<int, char>();
    entt::entity entities[3u];

    registry.create(std::begin(entities), std::end(entities));

    registry.emplace<int>(entities[0u]);
    registry.emplace<char>(entities[0u]);
    registry.emplace<double>(entities[0u]);

    registry.emplace<int>(entities[1u]);
    registry.emplace<char>(entities[1u]);

    registry.emplace<int>(entities[2u]);

    ASSERT_TRUE(registry.valid(entities[0u]));
    ASSERT_TRUE(registry.valid(entities[1u]));
    ASSERT_TRUE(registry.valid(entities[2u]));

    registry.destroy(icview.begin(), icview.end());

    ASSERT_FALSE(registry.valid(entities[0u]));
    ASSERT_FALSE(registry.valid(entities[1u]));
    ASSERT_TRUE(registry.valid(entities[2u]));

    ASSERT_EQ(registry.storage<int>().size(), 1u);
    ASSERT_EQ(registry.storage<char>().size(), 0u);
    ASSERT_EQ(registry.storage<double>().size(), 0u);

    registry.destroy(iview.begin(), iview.end());

    ASSERT_FALSE(registry.valid(entities[2u]));
    ASSERT_NO_FATAL_FAILURE(registry.destroy(iview.rbegin(), iview.rend()));
    ASSERT_EQ(iview.size(), 0u);
    ASSERT_EQ(icview.size_hint(), 0u);

    ASSERT_EQ(registry.storage<int>().size(), 0u);
    ASSERT_EQ(registry.storage<char>().size(), 0u);
    ASSERT_EQ(registry.storage<double>().size(), 0u);

    registry.create(std::begin(entities), std::end(entities));
    registry.insert<int>(std::begin(entities), std::end(entities));

    ASSERT_TRUE(registry.valid(entities[0u]));
    ASSERT_TRUE(registry.valid(entities[1u]));
    ASSERT_TRUE(registry.valid(entities[2u]));
    ASSERT_EQ(registry.storage<int>().size(), 3u);

    registry.destroy(std::begin(entities), std::end(entities));

    ASSERT_FALSE(registry.valid(entities[0u]));
    ASSERT_FALSE(registry.valid(entities[1u]));
    ASSERT_FALSE(registry.valid(entities[2u]));
    ASSERT_EQ(registry.storage<int>().size(), 0u);

    entt::sparse_set managed{};

    registry.create(std::begin(entities), std::end(entities));
    managed.push(std::begin(entities), std::end(entities));
    registry.insert<int>(managed.begin(), managed.end());

    ASSERT_TRUE(registry.valid(managed[0u]));
    ASSERT_TRUE(registry.valid(managed[1u]));
    ASSERT_TRUE(registry.valid(managed[2u]));
    ASSERT_EQ(registry.storage<int>().size(), 3u);

    registry.destroy(managed.begin(), managed.end());

    ASSERT_FALSE(registry.valid(managed[0u]));
    ASSERT_FALSE(registry.valid(managed[1u]));
    ASSERT_FALSE(registry.valid(managed[2u]));
    ASSERT_EQ(registry.storage<int>().size(), 0u);
}

TEST(Registry, StableDestroy) {
    entt::registry registry;
    const auto iview = registry.view<int>();
    const auto icview = registry.view<int, stable_type>();
    entt::entity entities[3u];

    registry.create(std::begin(entities), std::end(entities));

    registry.emplace<int>(entities[0u]);
    registry.emplace<stable_type>(entities[0u]);
    registry.emplace<double>(entities[0u]);

    registry.emplace<int>(entities[1u]);
    registry.emplace<stable_type>(entities[1u]);

    registry.emplace<int>(entities[2u]);

    ASSERT_TRUE(registry.valid(entities[0u]));
    ASSERT_TRUE(registry.valid(entities[1u]));
    ASSERT_TRUE(registry.valid(entities[2u]));

    registry.destroy(icview.begin(), icview.end());

    ASSERT_FALSE(registry.valid(entities[0u]));
    ASSERT_FALSE(registry.valid(entities[1u]));
    ASSERT_TRUE(registry.valid(entities[2u]));

    ASSERT_EQ(registry.storage<int>().size(), 1u);
    ASSERT_EQ(registry.storage<stable_type>().size(), 2u);
    ASSERT_EQ(registry.storage<double>().size(), 0u);

    registry.destroy(iview.begin(), iview.end());

    ASSERT_FALSE(registry.valid(entities[2u]));
    ASSERT_EQ(iview.size(), 0u);
    ASSERT_EQ(icview.size_hint(), 0u);

    ASSERT_EQ(registry.storage<int>().size(), 0u);
    ASSERT_EQ(registry.storage<stable_type>().size(), 2u);
    ASSERT_EQ(registry.storage<double>().size(), 0u);
}

TEST(Registry, ReleaseVersion) {
    entt::registry registry;
    entt::entity entities[2u];

    registry.create(std::begin(entities), std::end(entities));

    ASSERT_EQ(registry.current(entities[0u]), 0u);
    ASSERT_EQ(registry.current(entities[1u]), 0u);

    registry.release(entities[0u]);
    registry.release(entities[1u], 3);

    ASSERT_EQ(registry.current(entities[0u]), 1u);
    ASSERT_EQ(registry.current(entities[1u]), 3u);
}

ENTT_DEBUG_TEST(RegistryDeathTest, ReleaseVersion) {
    entt::registry registry;
    entt::entity entity = registry.create();

    registry.release(entity);

    ASSERT_DEATH(registry.release(entity), "");
    ASSERT_DEATH(registry.release(entity, 3), "");
}

TEST(Registry, ReleaseRange) {
    entt::registry registry;
    entt::entity entities[3u];

    registry.create(std::begin(entities), std::end(entities));

    ASSERT_TRUE(registry.valid(entities[0u]));
    ASSERT_TRUE(registry.valid(entities[1u]));
    ASSERT_TRUE(registry.valid(entities[2u]));

    registry.release(std::begin(entities), std::end(entities) - 1u);

    ASSERT_FALSE(registry.valid(entities[0u]));
    ASSERT_FALSE(registry.valid(entities[1u]));
    ASSERT_TRUE(registry.valid(entities[2u]));

    registry.release(std::end(entities) - 1u, std::end(entities));

    ASSERT_FALSE(registry.valid(entities[2u]));
}

TEST(Registry, VersionOverflow) {
    using traits_type = entt::entt_traits<entt::entity>;

    entt::registry registry;
    const auto entity = registry.create();

    registry.release(entity);

    ASSERT_NE(registry.current(entity), traits_type::to_version(entity));
    ASSERT_NE(registry.current(entity), typename traits_type::version_type{});

    registry.release(registry.create(), traits_type::to_version(entt::tombstone) - 1u);
    registry.release(registry.create());

    ASSERT_EQ(registry.current(entity), traits_type::to_version(entity));
    ASSERT_EQ(registry.current(entity), typename traits_type::version_type{});
}

TEST(Registry, NullEntity) {
    entt::registry registry;
    const entt::entity entity = entt::null;

    ASSERT_FALSE(registry.valid(entity));
    ASSERT_NE(registry.create(entity), entity);
}

TEST(Registry, TombstoneVersion) {
    using traits_type = entt::entt_traits<entt::entity>;

    entt::registry registry;
    const entt::entity entity = entt::tombstone;

    ASSERT_FALSE(registry.valid(entity));

    const auto other = registry.create();
    const auto vers = traits_type::to_version(entity);
    const auto required = traits_type::construct(traits_type::to_entity(other), vers);

    ASSERT_NE(registry.release(other, vers), vers);
    ASSERT_NE(registry.create(required), required);
}

TEST(Registry, Each) {
    entt::registry registry;
    entt::registry::size_type tot;
    entt::registry::size_type match;

    static_cast<void>(registry.create());
    registry.emplace<int>(registry.create());
    static_cast<void>(registry.create());
    registry.emplace<int>(registry.create());
    static_cast<void>(registry.create());

    tot = 0u;
    match = 0u;

    registry.each([&](auto entity) {
        match += registry.all_of<int>(entity);
        static_cast<void>(registry.create());
        ++tot;
    });

    ASSERT_EQ(tot, 5u);
    ASSERT_EQ(match, 2u);

    tot = 0u;
    match = 0u;

    registry.each([&](auto entity) {
        if(registry.all_of<int>(entity)) {
            registry.destroy(entity);
            ++match;
        }

        ++tot;
    });

    ASSERT_EQ(tot, 10u);
    ASSERT_EQ(match, 2u);

    tot = 0u;
    match = 0u;

    registry.each([&](auto entity) {
        match += registry.all_of<int>(entity);
        registry.destroy(entity);
        ++tot;
    });

    ASSERT_EQ(tot, 8u);
    ASSERT_EQ(match, 0u);

    registry.each([&](auto) { FAIL(); });
}

TEST(Registry, Orphans) {
    entt::registry registry;
    entt::entity entities[3u]{};

    registry.create(std::begin(entities), std::end(entities));
    registry.emplace<int>(entities[0u]);
    registry.emplace<int>(entities[2u]);

    registry.each([&](const auto entt) {
        ASSERT_TRUE(entt != entities[1u] || registry.orphan(entt));
    });

    registry.erase<int>(entities[0u]);
    registry.erase<int>(entities[2u]);

    registry.each([&](const auto entt) {
        ASSERT_TRUE(registry.orphan(entt));
    });
}

TEST(Registry, View) {
    entt::registry registry;
    entt::entity entities[3u];

    auto iview = registry.view<int>();
    auto cview = registry.view<char>();
    auto mview = registry.view<int, char>();
    auto fview = registry.view<int>(entt::exclude<char>);

    registry.create(std::begin(entities), std::end(entities));

    registry.emplace<int>(entities[0u], 0);
    registry.emplace<char>(entities[0u], 'c');

    registry.emplace<int>(entities[1u], 0);

    registry.emplace<int>(entities[2u], 0);
    registry.emplace<char>(entities[2u], 'c');

    ASSERT_EQ(iview.size(), 3u);
    ASSERT_EQ(cview.size(), 2u);

    ASSERT_EQ(mview.size_hint(), 3u);
    ASSERT_EQ(fview.size_hint(), 3u);

    mview.refresh();
    fview.refresh();

    ASSERT_EQ(mview.size_hint(), 2u);
    ASSERT_EQ(fview.size_hint(), 3u);

    ASSERT_NE(mview.begin(), mview.end());
    ASSERT_NE(fview.begin(), fview.end());

    ASSERT_EQ(std::distance(mview.begin(), mview.end()), 2);
    ASSERT_EQ(std::distance(fview.begin(), fview.end()), 1);

    mview.each([&entities, first = true](auto entity, auto &&...) mutable {
        ASSERT_EQ(entity, entities[2u * first]);
        first = false;
    });

    fview.each([&entities](auto entity, auto &&...) {
        ASSERT_EQ(entity, entities[1u]);
    });
}

TEST(Registry, ExcludeOnlyView) {
    entt::registry registry;
    entt::entity entities[4u];

    auto view = registry.view<entt::entity>(entt::exclude<int>);

    registry.create(std::begin(entities), std::end(entities));

    registry.emplace<int>(entities[0u], 0);
    registry.emplace<int>(entities[2u], 0);
    registry.emplace<int>(entities[3u], 0);

    registry.destroy(entities[3u]);

    ASSERT_EQ(view.size_hint(), 4u);
    ASSERT_NE(view.begin(), view.end());

    // returns all matching identifiers, both in-use and available ones
    ASSERT_EQ(std::distance(view.begin(), view.end()), 2);

    // skips available identifiers automatically, only returns in-use elements
    view.each([&entities](auto entity, auto &&...) {
        ASSERT_EQ(entity, entities[1u]);
    });
}

TEST(Registry, NonOwningGroupInitOnFirstUse) {
    entt::registry registry;
    entt::entity entities[3u];

    registry.create(std::begin(entities), std::end(entities));
    registry.insert<int>(std::begin(entities), std::end(entities), 0);
    registry.emplace<char>(entities[0u], 'c');
    registry.emplace<char>(entities[2u], 'c');

    std::size_t cnt{};
    auto group = registry.group(entt::get<int, char>);
    group.each([&cnt](auto...) { ++cnt; });

    ASSERT_FALSE((registry.owned<int, char>()));
    ASSERT_EQ(cnt, 2u);
}

TEST(Registry, NonOwningGroupInitOnEmplace) {
    entt::registry registry;
    entt::entity entities[3u];
    auto group = registry.group(entt::get<int, char>);

    registry.create(std::begin(entities), std::end(entities));
    registry.insert<int>(std::begin(entities), std::end(entities), 0);
    registry.emplace<char>(entities[0u], 'c');
    registry.emplace<char>(entities[2u], 'c');

    std::size_t cnt{};
    group.each([&cnt](auto...) { ++cnt; });

    ASSERT_FALSE((registry.owned<int, char>()));
    ASSERT_EQ(cnt, 2u);
}

TEST(Registry, FullOwningGroupInitOnFirstUse) {
    entt::registry registry;
    entt::entity entities[3u];

    registry.create(std::begin(entities), std::end(entities));
    registry.insert<int>(std::begin(entities), std::end(entities), 0);
    registry.emplace<char>(entities[0u], 'c');
    registry.emplace<char>(entities[2u], 'c');

    std::size_t cnt{};
    auto group = registry.group<int, char>();
    group.each([&cnt](auto...) { ++cnt; });

    ASSERT_TRUE(registry.owned<int>());
    ASSERT_TRUE(registry.owned<char>());
    ASSERT_FALSE(registry.owned<double>());
    ASSERT_EQ(cnt, 2u);
}

TEST(Registry, FullOwningGroupInitOnEmplace) {
    entt::registry registry;
    entt::entity entities[3u];
    auto group = registry.group<int, char>();

    registry.create(std::begin(entities), std::end(entities));
    registry.insert<int>(std::begin(entities), std::end(entities), 0);
    registry.emplace<char>(entities[0u], 'c');
    registry.emplace<char>(entities[2u], 'c');

    std::size_t cnt{};
    group.each([&cnt](auto...) { ++cnt; });

    ASSERT_TRUE(registry.owned<int>());
    ASSERT_TRUE(registry.owned<char>());
    ASSERT_FALSE(registry.owned<double>());
    ASSERT_EQ(cnt, 2u);
}

TEST(Registry, PartialOwningGroupInitOnFirstUse) {
    entt::registry registry;
    entt::entity entities[3u];

    registry.create(std::begin(entities), std::end(entities));
    registry.insert<int>(std::begin(entities), std::end(entities), 0);
    registry.emplace<char>(entities[0u], 'c');
    registry.emplace<char>(entities[2u], 'c');

    std::size_t cnt{};
    auto group = registry.group<int>(entt::get<char>);
    group.each([&cnt](auto...) { ++cnt; });

    ASSERT_TRUE((registry.owned<int, char>()));
    ASSERT_TRUE(registry.owned<int>());
    ASSERT_FALSE(registry.owned<char>());
    ASSERT_EQ(cnt, 2u);
}

TEST(Registry, PartialOwningGroupInitOnEmplace) {
    entt::registry registry;
    entt::entity entities[3u];
    auto group = registry.group<int>(entt::get<char>);

    registry.create(std::begin(entities), std::end(entities));
    registry.insert<int>(std::begin(entities), std::end(entities), 0);
    registry.emplace<char>(entities[0u], 'c');
    registry.emplace<char>(entities[2u], 'c');

    std::size_t cnt{};
    group.each([&cnt](auto...) { ++cnt; });

    ASSERT_TRUE((registry.owned<int, char>()));
    ASSERT_TRUE(registry.owned<int>());
    ASSERT_FALSE(registry.owned<char>());
    ASSERT_EQ(cnt, 2u);
}

TEST(Registry, CleanViewAfterRemoveAndClear) {
    entt::registry registry;
    auto view = registry.view<int, char>();

    const auto entity = registry.create();
    registry.emplace<int>(entity);
    registry.emplace<char>(entity);

    ASSERT_EQ(view.size_hint(), 1u);

    registry.erase<char>(entity);

    ASSERT_EQ(view.size_hint(), 1u);

    registry.emplace<char>(entity);

    ASSERT_EQ(view.size_hint(), 1u);

    registry.clear<int>();

    ASSERT_EQ(view.size_hint(), 0u);

    registry.emplace<int>(entity);

    ASSERT_EQ(view.size_hint(), 1u);

    registry.clear();

    ASSERT_EQ(view.size_hint(), 0u);
}

TEST(Registry, CleanNonOwningGroupViewAfterRemoveAndClear) {
    entt::registry registry;
    auto group = registry.group(entt::get<int, char>);

    const auto entity = registry.create();
    registry.emplace<int>(entity, 0);
    registry.emplace<char>(entity, 'c');

    ASSERT_EQ(group.size(), 1u);

    registry.erase<char>(entity);

    ASSERT_EQ(group.size(), 0u);

    registry.emplace<char>(entity, 'c');

    ASSERT_EQ(group.size(), 1u);

    registry.clear<int>();

    ASSERT_EQ(group.size(), 0u);

    registry.emplace<int>(entity, 0);

    ASSERT_EQ(group.size(), 1u);

    registry.clear();

    ASSERT_EQ(group.size(), 0u);
}

TEST(Registry, CleanFullOwningGroupViewAfterRemoveAndClear) {
    entt::registry registry;
    auto group = registry.group<int, char>();

    const auto entity = registry.create();
    registry.emplace<int>(entity, 0);
    registry.emplace<char>(entity, 'c');

    ASSERT_EQ(group.size(), 1u);

    registry.erase<char>(entity);

    ASSERT_EQ(group.size(), 0u);

    registry.emplace<char>(entity, 'c');

    ASSERT_EQ(group.size(), 1u);

    registry.clear<int>();

    ASSERT_EQ(group.size(), 0u);

    registry.emplace<int>(entity, 0);

    ASSERT_EQ(group.size(), 1u);

    registry.clear();

    ASSERT_EQ(group.size(), 0u);
}

TEST(Registry, CleanPartialOwningGroupViewAfterRemoveAndClear) {
    entt::registry registry;
    auto group = registry.group<int>(entt::get<char>);

    const auto entity = registry.create();
    registry.emplace<int>(entity, 0);
    registry.emplace<char>(entity, 'c');

    ASSERT_EQ(group.size(), 1u);

    registry.erase<char>(entity);

    ASSERT_EQ(group.size(), 0u);

    registry.emplace<char>(entity, 'c');

    ASSERT_EQ(group.size(), 1u);

    registry.clear<int>();

    ASSERT_EQ(group.size(), 0u);

    registry.emplace<int>(entity, 0);

    ASSERT_EQ(group.size(), 1u);

    registry.clear();

    ASSERT_EQ(group.size(), 0u);
}

ENTT_DEBUG_TEST(RegistryDeathTest, NestedGroups) {
    entt::registry registry;
    registry.group<int, double>(entt::get<char>);

    ASSERT_DEATH(registry.group<int>(entt::get<char>), "");
    ASSERT_DEATH(registry.group<int>(entt::get<char, double>), "");
    ASSERT_DEATH(registry.group<int>(entt::get<char>, entt::exclude<double>), "");
    ASSERT_DEATH((registry.group<int, double>()), "");
}

ENTT_DEBUG_TEST(RegistryDeathTest, ConflictingGroups) {
    entt::registry registry;

    registry.group<char>(entt::get<int>, entt::exclude<double>);
    ASSERT_DEATH(registry.group<char>(entt::get<float>, entt::exclude<double>), "");
}

TEST(Registry, SortSingle) {
    entt::registry registry;

    int val = 0;

    registry.emplace<int>(registry.create(), val++);
    registry.emplace<int>(registry.create(), val++);
    registry.emplace<int>(registry.create(), val++);

    for(auto entity: registry.view<int>()) {
        ASSERT_EQ(registry.get<int>(entity), --val);
    }

    registry.sort<int>(std::less<int>{});

    for(auto entity: registry.view<int>()) {
        ASSERT_EQ(registry.get<int>(entity), val++);
    }
}

TEST(Registry, SortMulti) {
    entt::registry registry;

    unsigned int uval = 0u;
    int ival = 0;

    for(auto i = 0; i < 3; ++i) {
        const auto entity = registry.create();
        registry.emplace<unsigned int>(entity, uval++);
        registry.emplace<int>(entity, ival++);
    }

    for(auto entity: registry.view<unsigned int>()) {
        ASSERT_EQ(registry.get<unsigned int>(entity), --uval);
    }

    for(auto entity: registry.view<int>()) {
        ASSERT_EQ(registry.get<int>(entity), --ival);
    }

    registry.sort<unsigned int>(std::less<unsigned int>{});
    registry.sort<int, unsigned int>();

    for(auto entity: registry.view<unsigned int>()) {
        ASSERT_EQ(registry.get<unsigned int>(entity), uval++);
    }

    for(auto entity: registry.view<int>()) {
        ASSERT_EQ(registry.get<int>(entity), ival++);
    }
}

TEST(Registry, SortEmpty) {
    entt::registry registry;

    registry.emplace<empty_type>(registry.create());
    registry.emplace<empty_type>(registry.create());
    registry.emplace<empty_type>(registry.create());

    ASSERT_LT(registry.storage<empty_type>().data()[0], registry.storage<empty_type>().data()[1]);
    ASSERT_LT(registry.storage<empty_type>().data()[1], registry.storage<empty_type>().data()[2]);

    registry.sort<empty_type>(std::less<entt::entity>{});

    ASSERT_GT(registry.storage<empty_type>().data()[0], registry.storage<empty_type>().data()[1]);
    ASSERT_GT(registry.storage<empty_type>().data()[1], registry.storage<empty_type>().data()[2]);
}

TEST(Registry, ComponentsWithTypesFromStandardTemplateLibrary) {
    // see #37 - the test shouldn't crash, that's all
    entt::registry registry;
    const auto entity = registry.create();
    registry.emplace<std::unordered_set<int>>(entity).insert(42);
    registry.destroy(entity);
}

TEST(Registry, ConstructWithComponents) {
    // it should compile, that's all
    entt::registry registry;
    const auto value = 0;
    registry.emplace<int>(registry.create(), value);
}

TEST(Registry, Signals) {
    entt::registry registry;
    entt::entity entities[2u];
    listener listener;

    registry.on_construct<empty_type>().connect<&listener::incr>(listener);
    registry.on_destroy<empty_type>().connect<&listener::decr>(listener);
    registry.on_construct<int>().connect<&listener::incr>(listener);
    registry.on_destroy<int>().connect<&listener::decr>(listener);

    registry.create(std::begin(entities), std::end(entities));
    registry.insert<empty_type>(std::begin(entities), std::end(entities));

    ASSERT_EQ(listener.counter, 2);
    ASSERT_EQ(listener.last, entities[1u]);

    registry.insert<int>(std::rbegin(entities), std::rend(entities));

    ASSERT_EQ(listener.counter, 4);
    ASSERT_EQ(listener.last, entities[0u]);

    registry.erase<empty_type, int>(entities[0u]);

    ASSERT_EQ(listener.counter, 2);
    ASSERT_EQ(listener.last, entities[0u]);

    registry.on_destroy<empty_type>().disconnect<&listener::decr>(listener);
    registry.on_destroy<int>().disconnect<&listener::decr>(listener);

    registry.erase<empty_type, int>(entities[1u]);

    ASSERT_EQ(listener.counter, 2);
    ASSERT_EQ(listener.last, entities[0u]);

    registry.on_construct<empty_type>().disconnect<&listener::incr>(listener);
    registry.on_construct<int>().disconnect<&listener::incr>(listener);

    registry.emplace<empty_type>(entities[1u]);
    registry.emplace<int>(entities[1u]);

    ASSERT_EQ(listener.counter, 2);
    ASSERT_EQ(listener.last, entities[0u]);

    registry.on_construct<int>().connect<&listener::incr>(listener);
    registry.on_destroy<int>().connect<&listener::decr>(listener);

    registry.emplace<int>(entities[0u]);
    registry.erase<int>(entities[1u]);

    ASSERT_EQ(listener.counter, 2);
    ASSERT_EQ(listener.last, entities[1u]);

    registry.on_construct<empty_type>().connect<&listener::incr>(listener);
    registry.on_destroy<empty_type>().connect<&listener::decr>(listener);

    registry.erase<empty_type>(entities[1u]);
    registry.emplace<empty_type>(entities[0u]);

    ASSERT_EQ(listener.counter, 2);
    ASSERT_EQ(listener.last, entities[0u]);

    registry.clear<empty_type, int>();

    ASSERT_EQ(listener.counter, 0);
    ASSERT_EQ(listener.last, entities[0u]);

    registry.insert<empty_type>(std::begin(entities), std::end(entities));
    registry.insert<int>(std::begin(entities), std::end(entities));
    registry.destroy(entities[1u]);

    ASSERT_EQ(listener.counter, 2);
    ASSERT_EQ(listener.last, entities[1u]);

    registry.erase<int, empty_type>(entities[0u]);
    registry.emplace_or_replace<int>(entities[0u]);
    registry.emplace_or_replace<empty_type>(entities[0u]);

    ASSERT_EQ(listener.counter, 2);
    ASSERT_EQ(listener.last, entities[0u]);

    registry.on_destroy<empty_type>().disconnect<&listener::decr>(listener);
    registry.on_destroy<int>().disconnect<&listener::decr>(listener);

    registry.emplace_or_replace<empty_type>(entities[0u]);
    registry.emplace_or_replace<int>(entities[0u]);

    ASSERT_EQ(listener.counter, 2);
    ASSERT_EQ(listener.last, entities[0u]);

    registry.on_update<empty_type>().connect<&listener::incr>(listener);
    registry.on_update<int>().connect<&listener::incr>(listener);

    registry.emplace_or_replace<empty_type>(entities[0u]);
    registry.emplace_or_replace<int>(entities[0u]);

    ASSERT_EQ(listener.counter, 4);
    ASSERT_EQ(listener.last, entities[0u]);

    registry.replace<empty_type>(entities[0u]);
    registry.replace<int>(entities[0u]);

    ASSERT_EQ(listener.counter, 6);
    ASSERT_EQ(listener.last, entities[0u]);
}

TEST(Registry, SignalsOnRuntimePool) {
    using namespace entt::literals;

    entt::registry registry;
    const auto entity = registry.create();
    listener listener;

    registry.on_construct<int>("custom"_hs).connect<&listener::incr>(listener);
    registry.on_update<int>("custom"_hs).connect<&listener::incr>(listener);
    registry.on_destroy<int>("custom"_hs).connect<&listener::incr>(listener);

    ASSERT_EQ(listener.counter, 0);

    registry.emplace<int>(entity);
    registry.patch<int>(entity);
    registry.erase<int>(entity);

    ASSERT_EQ(listener.counter, 0);

    registry.storage<int>("custom"_hs).emplace(entity);
    registry.storage<int>("custom"_hs).patch(entity);
    registry.storage<int>("custom"_hs).erase(entity);

    ASSERT_EQ(listener.counter, 3);
}

TEST(Registry, SignalsOnEntity) {
    entt::registry registry;
    listener listener;

    registry.on_construct<entt::entity>().connect<&listener::incr>(listener);

    entt::entity entity = registry.create();
    entt::entity other = registry.create();

    ASSERT_EQ(listener.counter, 2);
    ASSERT_EQ(listener.last, other);

    registry.destroy(other);
    registry.destroy(entity);

    ASSERT_EQ(listener.counter, 2);
    ASSERT_EQ(listener.last, other);

    registry.on_construct<entt::entity>().disconnect(&listener);

    other = registry.create();
    entity = registry.create();

    ASSERT_EQ(listener.counter, 2);
    ASSERT_NE(listener.last, entity);
    ASSERT_NE(listener.last, other);

    registry.on_update<entt::entity>().connect<&listener::decr>(listener);
    registry.patch<entt::entity>(entity);

    ASSERT_EQ(listener.counter, 1);
    ASSERT_EQ(listener.last, entity);

    registry.on_update<entt::entity>().disconnect(&listener);
    registry.patch<entt::entity>(other);

    ASSERT_EQ(listener.counter, 1);
    ASSERT_NE(listener.last, other);

    registry.on_destroy<entt::entity>().connect<&listener::decr>(listener);
    registry.destroy(entity);

    ASSERT_EQ(listener.counter, 0);
    ASSERT_EQ(listener.last, entity);

    registry.on_destroy<entt::entity>().disconnect(&listener);
    registry.destroy(other);

    ASSERT_EQ(listener.counter, 0);
    ASSERT_NE(listener.last, other);
}

TEST(Registry, SignalWhenDestroying) {
    entt::registry registry;
    const auto entity = registry.create();

    registry.on_destroy<double>().connect<&entt::registry::remove<char>>();
    registry.emplace<double>(entity);
    registry.emplace<int>(entity);

    ASSERT_NE(registry.storage(entt::type_id<double>().hash()), nullptr);
    ASSERT_NE(registry.storage(entt::type_id<int>().hash()), nullptr);
    ASSERT_EQ(registry.storage(entt::type_id<char>().hash()), nullptr);
    ASSERT_TRUE(registry.valid(entity));

    registry.destroy(entity);

    ASSERT_NE(registry.storage(entt::type_id<char>().hash()), nullptr);
    ASSERT_FALSE(registry.valid(entity));
}

TEST(Registry, Insert) {
    entt::registry registry;
    entt::entity entities[3u];

    registry.create(std::begin(entities), std::end(entities));

    registry.emplace<int>(entities[0u]);
    registry.emplace<char>(entities[0u]);
    registry.emplace<double>(entities[0u]);

    registry.emplace<int>(entities[1u]);
    registry.emplace<char>(entities[1u]);

    registry.emplace<int>(entities[2u]);

    ASSERT_FALSE(registry.all_of<float>(entities[0u]));
    ASSERT_FALSE(registry.all_of<float>(entities[1u]));
    ASSERT_FALSE(registry.all_of<float>(entities[2u]));

    const auto icview = registry.view<int, char>();
    registry.insert(icview.begin(), icview.end(), 3.f);

    ASSERT_EQ(registry.get<float>(entities[0u]), 3.f);
    ASSERT_EQ(registry.get<float>(entities[1u]), 3.f);
    ASSERT_FALSE(registry.all_of<float>(entities[2u]));

    registry.clear<float>();
    float value[3]{0.f, 1.f, 2.f};

    const auto iview = registry.view<int>();
    registry.insert<float>(iview.rbegin(), iview.rend(), value);

    ASSERT_EQ(registry.get<float>(entities[0u]), 0.f);
    ASSERT_EQ(registry.get<float>(entities[1u]), 1.f);
    ASSERT_EQ(registry.get<float>(entities[2u]), 2.f);
}

TEST(Registry, Erase) {
    entt::registry registry;
    const auto iview = registry.view<int>();
    const auto icview = registry.view<int, char>();
    entt::entity entities[3u];

    registry.create(std::begin(entities), std::end(entities));

    registry.emplace<int>(entities[0u]);
    registry.emplace<char>(entities[0u]);
    registry.emplace<double>(entities[0u]);

    registry.emplace<int>(entities[1u]);
    registry.emplace<char>(entities[1u]);

    registry.emplace<int>(entities[2u]);

    ASSERT_TRUE(registry.any_of<int>(entities[0u]));
    ASSERT_TRUE(registry.all_of<int>(entities[1u]));
    ASSERT_TRUE(registry.any_of<int>(entities[2u]));

    registry.erase<int, char>(entities[0u]);
    registry.erase<int, char>(icview.begin(), icview.end());

    ASSERT_FALSE(registry.any_of<int>(entities[0u]));
    ASSERT_FALSE(registry.all_of<int>(entities[1u]));
    ASSERT_TRUE(registry.any_of<int>(entities[2u]));

    ASSERT_EQ(registry.storage<int>().size(), 1u);
    ASSERT_EQ(registry.storage<char>().size(), 0u);
    ASSERT_EQ(registry.storage<double>().size(), 1u);

    registry.erase<int>(iview.begin(), iview.end());

    ASSERT_FALSE(registry.any_of<int>(entities[2u]));
    ASSERT_NO_FATAL_FAILURE(registry.erase<int>(iview.rbegin(), iview.rend()));

    ASSERT_EQ(registry.storage<int>().size(), 0u);
    ASSERT_EQ(registry.storage<char>().size(), 0u);
    ASSERT_EQ(registry.storage<double>().size(), 1u);

    registry.insert<int>(std::begin(entities) + 1, std::end(entities) - 1u);
    registry.insert<char>(std::begin(entities) + 1, std::end(entities) - 1u);

    ASSERT_EQ(registry.storage<int>().size(), 1u);
    ASSERT_EQ(registry.storage<char>().size(), 1u);

    registry.erase<int, char>(iview.begin(), iview.end());

    ASSERT_EQ(registry.storage<int>().size(), 0u);
    ASSERT_EQ(registry.storage<char>().size(), 0u);

    registry.insert<int>(std::begin(entities), std::end(entities));
    registry.insert<char>(std::begin(entities), std::end(entities));

    ASSERT_EQ(registry.storage<int>().size(), 3u);
    ASSERT_EQ(registry.storage<char>().size(), 3u);

    registry.erase<int, char>(std::begin(entities), std::end(entities));

    ASSERT_EQ(registry.storage<int>().size(), 0u);
    ASSERT_EQ(registry.storage<char>().size(), 0u);

    ASSERT_FALSE(registry.orphan(entities[0u]));
    ASSERT_TRUE(registry.orphan(entities[1u]));
    ASSERT_TRUE(registry.orphan(entities[2u]));
}

ENTT_DEBUG_TEST(RegistryDeathTest, Erase) {
    entt::registry registry;
    const entt::entity entities[1u]{registry.create()};

    ASSERT_FALSE((registry.any_of<int>(entities[0u])));
    ASSERT_DEATH((registry.erase<int>(std::begin(entities), std::end(entities))), "");
    ASSERT_DEATH(registry.erase<int>(entities[0u]), "");
}

TEST(Registry, StableErase) {
    entt::registry registry;
    const auto iview = registry.view<int>();
    const auto icview = registry.view<int, stable_type>();
    entt::entity entities[3u];

    registry.create(std::begin(entities), std::end(entities));

    registry.emplace<int>(entities[0u]);
    registry.emplace<stable_type>(entities[0u]);
    registry.emplace<double>(entities[0u]);

    registry.emplace<int>(entities[1u]);
    registry.emplace<stable_type>(entities[1u]);

    registry.emplace<int>(entities[2u]);

    ASSERT_TRUE(registry.any_of<int>(entities[0u]));
    ASSERT_TRUE(registry.all_of<int>(entities[1u]));
    ASSERT_TRUE(registry.any_of<int>(entities[2u]));

    registry.erase<int, stable_type>(entities[0u]);
    registry.erase<int, stable_type>(icview.begin(), icview.end());
    registry.erase<int, stable_type>(icview.begin(), icview.end());

    ASSERT_FALSE(registry.any_of<int>(entities[0u]));
    ASSERT_FALSE(registry.all_of<int>(entities[1u]));
    ASSERT_TRUE(registry.any_of<int>(entities[2u]));

    ASSERT_EQ(registry.storage<int>().size(), 1u);
    ASSERT_EQ(registry.storage<stable_type>().size(), 2u);
    ASSERT_EQ(registry.storage<double>().size(), 1u);

    registry.erase<int>(iview.begin(), iview.end());

    ASSERT_FALSE(registry.any_of<int>(entities[2u]));

    ASSERT_EQ(registry.storage<int>().size(), 0u);
    ASSERT_EQ(registry.storage<stable_type>().size(), 2u);
    ASSERT_EQ(registry.storage<double>().size(), 1u);
}

TEST(Registry, EraseIf) {
    using namespace entt::literals;

    entt::registry registry;
    const auto entity = registry.create();

    registry.emplace<int>(entity);
    registry.storage<int>("other"_hs).emplace(entity);
    registry.emplace<char>(entity);

    ASSERT_TRUE(registry.storage<int>().contains(entity));
    ASSERT_TRUE(registry.storage<int>("other"_hs).contains(entity));
    ASSERT_TRUE(registry.storage<char>().contains(entity));

    registry.erase_if(entity, [](auto &&...) { return false; });

    ASSERT_TRUE(registry.storage<int>().contains(entity));
    ASSERT_TRUE(registry.storage<int>("other"_hs).contains(entity));
    ASSERT_TRUE(registry.storage<char>().contains(entity));

    registry.erase_if(entity, [](entt::id_type id, auto &&...) { return id == "other"_hs; });

    ASSERT_TRUE(registry.storage<int>().contains(entity));
    ASSERT_FALSE(registry.storage<int>("other"_hs).contains(entity));
    ASSERT_TRUE(registry.storage<char>().contains(entity));

    registry.erase_if(entity, [](auto, const auto &storage) { return storage.type() == entt::type_id<char>(); });

    ASSERT_TRUE(registry.storage<int>().contains(entity));
    ASSERT_FALSE(registry.storage<int>("other"_hs).contains(entity));
    ASSERT_FALSE(registry.storage<char>().contains(entity));
}

TEST(Registry, Remove) {
    entt::registry registry;
    const auto iview = registry.view<int>();
    const auto icview = registry.view<int, char>();
    entt::entity entities[3u];

    registry.create(std::begin(entities), std::end(entities));

    registry.emplace<int>(entities[0u]);
    registry.emplace<char>(entities[0u]);
    registry.emplace<double>(entities[0u]);

    registry.emplace<int>(entities[1u]);
    registry.emplace<char>(entities[1u]);

    registry.emplace<int>(entities[2u]);

    ASSERT_TRUE(registry.any_of<int>(entities[0u]));
    ASSERT_TRUE(registry.all_of<int>(entities[1u]));
    ASSERT_TRUE(registry.any_of<int>(entities[2u]));

    registry.remove<int, char>(entities[0u]);

    ASSERT_EQ((registry.remove<int, char>(icview.begin(), icview.end())), 2u);
    ASSERT_EQ((registry.remove<int, char>(icview.begin(), icview.end())), 0u);

    ASSERT_FALSE(registry.any_of<int>(entities[0u]));
    ASSERT_FALSE(registry.all_of<int>(entities[1u]));
    ASSERT_TRUE(registry.any_of<int>(entities[2u]));

    ASSERT_EQ(registry.storage<int>().size(), 1u);
    ASSERT_EQ(registry.storage<char>().size(), 0u);
    ASSERT_EQ(registry.storage<double>().size(), 1u);

    ASSERT_EQ((registry.remove<int>(iview.begin(), iview.end())), 1u);

    ASSERT_EQ(registry.remove<int>(entities[0u]), 0u);
    ASSERT_EQ(registry.remove<int>(entities[1u]), 0u);

    ASSERT_FALSE(registry.any_of<int>(entities[2u]));
    ASSERT_EQ(registry.remove<int>(iview.begin(), iview.end()), 0u);

    ASSERT_EQ(registry.storage<int>().size(), 0u);
    ASSERT_EQ(registry.storage<char>().size(), 0u);
    ASSERT_EQ(registry.storage<double>().size(), 1u);

    registry.insert<int>(std::begin(entities) + 1, std::end(entities) - 1u);
    registry.insert<char>(std::begin(entities) + 1, std::end(entities) - 1u);

    ASSERT_EQ(registry.storage<int>().size(), 1u);
    ASSERT_EQ(registry.storage<char>().size(), 1u);

    registry.remove<int, char>(iview.begin(), iview.end());
    registry.remove<int, char>(iview.begin(), iview.end());

    ASSERT_EQ(registry.storage<int>().size(), 0u);
    ASSERT_EQ(registry.storage<char>().size(), 0u);

    registry.insert<int>(std::begin(entities), std::end(entities));
    registry.insert<char>(std::begin(entities), std::end(entities));

    ASSERT_EQ(registry.storage<int>().size(), 3u);
    ASSERT_EQ(registry.storage<char>().size(), 3u);

    registry.remove<int, char>(std::begin(entities), std::end(entities));
    registry.remove<int, char>(std::begin(entities), std::end(entities));

    ASSERT_EQ(registry.storage<int>().size(), 0u);
    ASSERT_EQ(registry.storage<char>().size(), 0u);

    ASSERT_FALSE(registry.orphan(entities[0u]));
    ASSERT_TRUE(registry.orphan(entities[1u]));
    ASSERT_TRUE(registry.orphan(entities[2u]));
}

TEST(Registry, StableRemove) {
    entt::registry registry;
    const auto iview = registry.view<int>();
    const auto icview = registry.view<int, stable_type>();
    entt::entity entities[3u];

    registry.create(std::begin(entities), std::end(entities));

    registry.emplace<int>(entities[0u]);
    registry.emplace<stable_type>(entities[0u]);
    registry.emplace<double>(entities[0u]);

    registry.emplace<int>(entities[1u]);
    registry.emplace<stable_type>(entities[1u]);

    registry.emplace<int>(entities[2u]);

    ASSERT_TRUE(registry.any_of<int>(entities[0u]));
    ASSERT_TRUE(registry.all_of<int>(entities[1u]));
    ASSERT_TRUE(registry.any_of<int>(entities[2u]));

    registry.remove<int, stable_type>(entities[0u]);

    ASSERT_EQ((registry.remove<int, stable_type>(icview.begin(), icview.end())), 2u);
    ASSERT_EQ((registry.remove<int, stable_type>(icview.begin(), icview.end())), 0u);

    ASSERT_FALSE(registry.any_of<int>(entities[0u]));
    ASSERT_FALSE(registry.all_of<int>(entities[1u]));
    ASSERT_TRUE(registry.any_of<int>(entities[2u]));

    ASSERT_EQ(registry.storage<int>().size(), 1u);
    ASSERT_EQ(registry.storage<stable_type>().size(), 2u);
    ASSERT_EQ(registry.storage<double>().size(), 1u);

    ASSERT_EQ((registry.remove<int>(iview.begin(), iview.end())), 1u);

    ASSERT_EQ(registry.remove<int>(entities[0u]), 0u);
    ASSERT_EQ(registry.remove<int>(entities[1u]), 0u);

    ASSERT_FALSE(registry.any_of<int>(entities[2u]));

    ASSERT_EQ(registry.storage<int>().size(), 0u);
    ASSERT_EQ(registry.storage<stable_type>().size(), 2u);
    ASSERT_EQ(registry.storage<double>().size(), 1u);
}

TEST(Registry, Compact) {
    entt::registry registry;
    entt::entity entities[2u];

    registry.create(std::begin(entities), std::end(entities));

    registry.emplace<int>(entities[0u]);
    registry.emplace<stable_type>(entities[0u]);

    registry.emplace<int>(entities[1u]);
    registry.emplace<stable_type>(entities[1u]);

    ASSERT_EQ(registry.storage<int>().size(), 2u);
    ASSERT_EQ(registry.storage<stable_type>().size(), 2u);

    registry.destroy(std::begin(entities), std::end(entities));

    ASSERT_EQ(registry.storage<int>().size(), 0u);
    ASSERT_EQ(registry.storage<stable_type>().size(), 2u);

    registry.compact<int>();

    ASSERT_EQ(registry.storage<int>().size(), 0u);
    ASSERT_EQ(registry.storage<stable_type>().size(), 2u);

    registry.compact();

    ASSERT_EQ(registry.storage<int>().size(), 0u);
    ASSERT_EQ(registry.storage<stable_type>().size(), 0u);
}

TEST(Registry, NonOwningGroupInterleaved) {
    entt::registry registry;
    typename entt::entity entity = entt::null;

    entity = registry.create();
    registry.emplace<int>(entity);
    registry.emplace<char>(entity);

    const auto group = registry.group(entt::get<int, char>);

    entity = registry.create();
    registry.emplace<int>(entity);
    registry.emplace<char>(entity);

    std::size_t cnt{};
    group.each([&cnt](auto...) { ++cnt; });

    ASSERT_EQ(cnt, 2u);
}

TEST(Registry, FullOwningGroupInterleaved) {
    entt::registry registry;
    typename entt::entity entity = entt::null;

    entity = registry.create();
    registry.emplace<int>(entity);
    registry.emplace<char>(entity);

    const auto group = registry.group<int, char>();

    entity = registry.create();
    registry.emplace<int>(entity);
    registry.emplace<char>(entity);

    std::size_t cnt{};
    group.each([&cnt](auto...) { ++cnt; });

    ASSERT_EQ(cnt, 2u);
}

TEST(Registry, PartialOwningGroupInterleaved) {
    entt::registry registry;
    typename entt::entity entity = entt::null;

    entity = registry.create();
    registry.emplace<int>(entity);
    registry.emplace<char>(entity);

    const auto group = registry.group<int>(entt::get<char>);

    entity = registry.create();
    registry.emplace<int>(entity);
    registry.emplace<char>(entity);

    std::size_t cnt{};
    group.each([&cnt](auto...) { ++cnt; });

    ASSERT_EQ(cnt, 2u);
}

TEST(Registry, NonOwningGroupSortInterleaved) {
    entt::registry registry;
    const auto group = registry.group(entt::get<int, char>);

    const auto e0 = registry.create();
    registry.emplace<int>(e0, 0);
    registry.emplace<char>(e0, '0');

    const auto e1 = registry.create();
    registry.emplace<int>(e1, 1);
    registry.emplace<char>(e1, '1');

    registry.sort<int>(std::greater{});
    registry.sort<char>(std::less{});

    const auto e2 = registry.create();
    registry.emplace<int>(e2, 2);
    registry.emplace<char>(e2, '2');

    group.each([e0, e1, e2](const auto entity, const auto &i, const auto &c) {
        if(entity == e0) {
            ASSERT_EQ(i, 0);
            ASSERT_EQ(c, '0');
        } else if(entity == e1) {
            ASSERT_EQ(i, 1);
            ASSERT_EQ(c, '1');
        } else if(entity == e2) {
            ASSERT_EQ(i, 2);
            ASSERT_EQ(c, '2');
        }
    });
}

TEST(Registry, GetOrEmplace) {
    entt::registry registry;
    const auto entity = registry.create();
    const auto value = registry.get_or_emplace<int>(entity, 3);
    // get_or_emplace must work for empty types
    static_cast<void>(registry.get_or_emplace<empty_type>(entity));

    ASSERT_TRUE((registry.all_of<int, empty_type>(entity)));
    ASSERT_EQ(registry.get<int>(entity), value);
    ASSERT_EQ(registry.get<int>(entity), 3);
}

TEST(Registry, Constness) {
    entt::registry registry;

    static_assert((std::is_same_v<decltype(registry.emplace<int>({})), int &>));
    static_assert((std::is_same_v<decltype(registry.emplace<empty_type>({})), void>));

    static_assert((std::is_same_v<decltype(registry.get<>({})), std::tuple<>>));
    static_assert((std::is_same_v<decltype(registry.get<int>({})), int &>));
    static_assert((std::is_same_v<decltype(registry.get<int, const char>({})), std::tuple<int &, const char &>>));

    static_assert((std::is_same_v<decltype(registry.try_get<>({})), std::tuple<>>));
    static_assert((std::is_same_v<decltype(registry.try_get<int>({})), int *>));
    static_assert((std::is_same_v<decltype(registry.try_get<int, const char>({})), std::tuple<int *, const char *>>));

    static_assert((std::is_same_v<decltype(registry.ctx().get<int>()), int &>));
    static_assert((std::is_same_v<decltype(registry.ctx().get<const char>()), const char &>));

    static_assert((std::is_same_v<decltype(registry.ctx().find<int>()), int *>));
    static_assert((std::is_same_v<decltype(registry.ctx().find<const char>()), const char *>));

    static_assert((std::is_same_v<decltype(std::as_const(registry).get<>({})), std::tuple<>>));
    static_assert((std::is_same_v<decltype(std::as_const(registry).get<int>({})), const int &>));
    static_assert((std::is_same_v<decltype(std::as_const(registry).get<int, const char>({})), std::tuple<const int &, const char &>>));

    static_assert((std::is_same_v<decltype(std::as_const(registry).try_get<>({})), std::tuple<>>));
    static_assert((std::is_same_v<decltype(std::as_const(registry).try_get<int>({})), const int *>));
    static_assert((std::is_same_v<decltype(std::as_const(registry).try_get<int, const char>({})), std::tuple<const int *, const char *>>));

    static_assert((std::is_same_v<decltype(std::as_const(registry).ctx().get<int>()), const int &>));
    static_assert((std::is_same_v<decltype(std::as_const(registry).ctx().get<const char>()), const char &>));

    static_assert((std::is_same_v<decltype(std::as_const(registry).ctx().find<int>()), const int *>));
    static_assert((std::is_same_v<decltype(std::as_const(registry).ctx().find<const char>()), const char *>));
}

TEST(Registry, MoveOnlyComponent) {
    entt::registry registry;
    // the purpose is to ensure that move only types are always accepted
    registry.emplace<std::unique_ptr<int>>(registry.create());
}

TEST(Registry, NonDefaultConstructibleComponent) {
    entt::registry registry;
    // the purpose is to ensure that non default constructible type are always accepted
    registry.emplace<non_default_constructible>(registry.create(), 42);
}

TEST(Registry, Dependencies) {
    entt::registry registry;
    const auto entity = registry.create();

    // required because of an issue of VS2019
    constexpr auto emplace_or_replace = &entt::registry::emplace_or_replace<double>;
    constexpr auto remove = &entt::registry::remove<double>;

    registry.on_construct<int>().connect<emplace_or_replace>();
    registry.on_destroy<int>().connect<remove>();
    registry.emplace<double>(entity, .3);

    ASSERT_FALSE(registry.all_of<int>(entity));
    ASSERT_EQ(registry.get<double>(entity), .3);

    registry.emplace<int>(entity);

    ASSERT_TRUE(registry.all_of<int>(entity));
    ASSERT_EQ(registry.get<double>(entity), .0);

    registry.erase<int>(entity);

    ASSERT_FALSE((registry.any_of<int, double>(entity)));

    registry.on_construct<int>().disconnect<emplace_or_replace>();
    registry.on_destroy<int>().disconnect<remove>();
    registry.emplace<int>(entity);

    ASSERT_TRUE((registry.any_of<int, double>(entity)));
    ASSERT_FALSE(registry.all_of<double>(entity));
}

TEST(Registry, StableEmplace) {
    entt::registry registry;
    registry.on_construct<int>().connect<&listener::sort<int>>();
    registry.emplace<int>(registry.create(), 0);

    ASSERT_EQ(registry.emplace<int>(registry.create(), 1), 1);
}

TEST(Registry, AssignEntities) {
    using traits_type = entt::entt_traits<entt::entity>;

    entt::registry registry;
    entt::entity entities[3];
    registry.create(std::begin(entities), std::end(entities));
    registry.release(entities[1]);
    registry.release(entities[2]);

    entt::registry other;
    const auto *data = registry.data();
    other.assign(data, data + registry.size(), registry.released());

    ASSERT_EQ(registry.size(), other.size());
    ASSERT_TRUE(other.valid(entities[0]));
    ASSERT_FALSE(other.valid(entities[1]));
    ASSERT_FALSE(other.valid(entities[2]));
    ASSERT_EQ(registry.create(), other.create());
    ASSERT_EQ(traits_type::to_entity(other.create()), traits_type::to_integral(entities[1]));
}

TEST(Registry, ScramblingPoolsIsAllowed) {
    entt::registry registry;
    registry.on_destroy<int>().connect<&listener::sort<int>>();

    for(std::size_t i{}; i < 2u; ++i) {
        const auto entity = registry.create();
        registry.emplace<int>(entity, static_cast<int>(i));
    }

    registry.destroy(registry.view<int>().back());

    // thanks to @andranik3949 for pointing out this missing test
    registry.view<const int>().each([](const auto entity, const auto &value) {
        ASSERT_EQ(static_cast<int>(entt::to_integral(entity)), value);
    });
}

TEST(Registry, RuntimePools) {
    using namespace entt::literals;

    entt::registry registry;
    auto &storage = registry.storage<empty_type>("other"_hs);
    const auto entity = registry.create();

    static_assert(std::is_same_v<decltype(registry.storage<empty_type>()), entt::storage_type_t<empty_type> &>);
    static_assert(std::is_same_v<decltype(std::as_const(registry).storage<empty_type>()), const entt::storage_type_t<empty_type> *>);

    static_assert(std::is_same_v<decltype(registry.storage("other"_hs)), entt::storage_type_t<empty_type>::base_type *>);
    static_assert(std::is_same_v<decltype(std::as_const(registry).storage("other"_hs)), const entt::storage_type_t<empty_type>::base_type *>);

    ASSERT_NE(registry.storage("other"_hs), nullptr);
    ASSERT_EQ(std::as_const(registry).storage("rehto"_hs), nullptr);

    ASSERT_EQ(&registry.storage<empty_type>("other"_hs), &storage);
    ASSERT_NE(std::as_const(registry).storage<empty_type>(), &storage);

    ASSERT_FALSE(registry.any_of<empty_type>(entity));
    ASSERT_FALSE(storage.contains(entity));

    registry.emplace<empty_type>(entity);

    ASSERT_FALSE(storage.contains(entity));
    ASSERT_TRUE(registry.any_of<empty_type>(entity));
    ASSERT_EQ((entt::basic_view{registry.storage<empty_type>(), storage}.size_hint()), 0u);

    storage.emplace(entity);

    ASSERT_TRUE(storage.contains(entity));
    ASSERT_TRUE(registry.any_of<empty_type>(entity));
    ASSERT_EQ((entt::basic_view{registry.storage<empty_type>(), storage}.size_hint()), 1u);

    registry.destroy(entity);

    ASSERT_EQ(registry.create(entity), entity);

    ASSERT_FALSE(storage.contains(entity));
    ASSERT_FALSE(registry.any_of<empty_type>(entity));
}

ENTT_DEBUG_TEST(RegistryDeathTest, RuntimePools) {
    using namespace entt::literals;

    entt::registry registry;
    registry.storage<empty_type>("other"_hs);

    ASSERT_DEATH(registry.storage<int>("other"_hs), "");
    ASSERT_DEATH(std::as_const(registry).storage<int>("other"_hs), "");
}

TEST(Registry, Storage) {
    using namespace entt::literals;

    entt::registry registry;
    const auto entity = registry.create();

    auto &storage = registry.storage<int>("int"_hs);
    storage.emplace(entity);

    for(auto [id, pool]: registry.storage()) {
        static_assert(std::is_same_v<decltype(pool), entt::sparse_set &>);
        static_assert(std::is_same_v<decltype(id), entt::id_type>);

        ASSERT_TRUE(pool.contains(entity));
        ASSERT_EQ(std::addressof(storage), std::addressof(pool));
        ASSERT_EQ(id, "int"_hs);
    }

    for(auto &&curr: std::as_const(registry).storage()) {
        static_assert(std::is_same_v<decltype(curr.second), const entt::sparse_set &>);
        static_assert(std::is_same_v<decltype(curr.first), entt::id_type>);

        ASSERT_TRUE(curr.second.contains(entity));
        ASSERT_EQ(std::addressof(storage), std::addressof(curr.second));
        ASSERT_EQ(curr.first, "int"_hs);
    }
}

TEST(Registry, RegistryStorageIterator) {
    entt::registry registry;
    const auto entity = registry.create();
    registry.emplace<int>(entity);

    auto test = [entity](auto iterable) {
        auto end{iterable.begin()};
        decltype(end) begin{};
        begin = iterable.end();
        std::swap(begin, end);

        ASSERT_EQ(begin, iterable.cbegin());
        ASSERT_EQ(end, iterable.cend());
        ASSERT_NE(begin, end);

        ASSERT_EQ(begin++, iterable.begin());
        ASSERT_EQ(begin--, iterable.end());

        ASSERT_EQ(begin + 1, iterable.end());
        ASSERT_EQ(end - 1, iterable.begin());

        ASSERT_EQ(++begin, iterable.end());
        ASSERT_EQ(--begin, iterable.begin());

        ASSERT_EQ(begin += 1, iterable.end());
        ASSERT_EQ(begin -= 1, iterable.begin());

        ASSERT_EQ(begin + (end - begin), iterable.end());
        ASSERT_EQ(begin - (begin - end), iterable.end());

        ASSERT_EQ(end - (end - begin), iterable.begin());
        ASSERT_EQ(end + (begin - end), iterable.begin());

        ASSERT_EQ(begin[0u].first, iterable.begin()->first);
        ASSERT_EQ(std::addressof(begin[0u].second), std::addressof((*iterable.begin()).second));

        ASSERT_LT(begin, end);
        ASSERT_LE(begin, iterable.begin());

        ASSERT_GT(end, begin);
        ASSERT_GE(end, iterable.end());

        ASSERT_EQ(begin[0u].first, entt::type_id<int>().hash());
        ASSERT_TRUE(begin[0u].second.contains(entity));
    };

    test(registry.storage());
    test(std::as_const(registry).storage());

    decltype(std::as_const(registry).storage().begin()) cit = registry.storage().begin();

    ASSERT_EQ(cit, registry.storage().begin());
    ASSERT_NE(cit, std::as_const(registry).storage().end());
}

TEST(Registry, RegistryStorageIteratorConversion) {
    entt::registry registry;
    registry.storage<int>();

    auto proxy = registry.storage();
    auto cproxy = std::as_const(registry).storage();

    typename decltype(proxy)::iterator it = proxy.begin();
    typename decltype(cproxy)::iterator cit = it;

    static_assert(std::is_same_v<decltype(*it), std::pair<entt::id_type, entt::sparse_set &>>);
    static_assert(std::is_same_v<decltype(*cit), std::pair<entt::id_type, const entt::sparse_set &>>);

    ASSERT_EQ(it->first, entt::type_id<int>().hash());
    ASSERT_EQ((*it).second.type(), entt::type_id<int>());
    ASSERT_EQ(it->first, cit->first);
    ASSERT_EQ((*it).second.type(), (*cit).second.type());

    ASSERT_EQ(it - cit, 0);
    ASSERT_EQ(cit - it, 0);
    ASSERT_LE(it, cit);
    ASSERT_LE(cit, it);
    ASSERT_GE(it, cit);
    ASSERT_GE(cit, it);
    ASSERT_EQ(it, cit);
    ASSERT_NE(++cit, it);
}

TEST(Registry, VoidType) {
    using namespace entt::literals;

    entt::registry registry;
    const auto entity = registry.create();
    auto &storage = registry.storage<void>("custom"_hs);
    storage.emplace(entity);

    ASSERT_TRUE(registry.storage<void>().empty());
    ASSERT_FALSE(registry.storage<void>("custom"_hs).empty());
    ASSERT_TRUE(registry.storage<void>("custom"_hs).contains(entity));

    ASSERT_EQ(registry.storage<void>().type(), entt::type_id<void>());
    ASSERT_EQ(registry.storage<void>("custom"_hs).type(), entt::type_id<void>());
}

TEST(Registry, NoEtoType) {
    entt::registry registry;
    const auto entity = registry.create();

    registry.emplace<no_eto_type>(entity);
    registry.emplace<int>(entity, 42);

    ASSERT_NE(registry.storage<no_eto_type>().raw(), nullptr);
    ASSERT_NE(registry.try_get<no_eto_type>(entity), nullptr);
    ASSERT_EQ(registry.view<no_eto_type>().get(entity), std::as_const(registry).view<const no_eto_type>().get(entity));

    auto view = registry.view<no_eto_type, int>();
    auto cview = std::as_const(registry).view<const no_eto_type, const int>();

    ASSERT_EQ((std::get<0>(view.get<no_eto_type, int>(entity))), (std::get<0>(cview.get<const no_eto_type, const int>(entity))));
}

TEST(Registry, CtxAndPoolMemberDestructionOrder) {
    auto registry = std::make_unique<entt::registry>();
    const auto entity = registry->create();
    bool ctx_check = false;

    registry->ctx().emplace<typename destruction_order::ctx_check_type>();
    registry->emplace<destruction_order>(entity, *registry, ctx_check);
    registry.reset();

    ASSERT_TRUE(ctx_check);
}
