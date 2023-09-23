#include <chrono>
#include <cstdint>
#include <iostream>
#include <utility>
#include <vector>
#include <gtest/gtest.h>
#include <entt/entity/registry.hpp>
#include <entt/entity/runtime_view.hpp>

struct position {
    std::uint64_t x;
    std::uint64_t y;
};

struct velocity: position {};

struct stable_position: position {
    static constexpr auto in_place_delete = true;
};

template<auto>
struct comp {
    int x;
};

struct timer final {
    timer()
        : start{std::chrono::system_clock::now()} {}

    void elapsed() {
        auto now = std::chrono::system_clock::now();
        std::cout << std::chrono::duration<double>(now - start).count() << " seconds" << std::endl;
    }

private:
    std::chrono::time_point<std::chrono::system_clock> start;
};

template<typename Func, typename... Args>
void generic_with(Func func) {
    timer timer;
    func();
    timer.elapsed();
}

template<typename Iterable, typename Func>
void iterate_with(Iterable &&iterable, Func func) {
    timer timer;
    std::forward<Iterable>(iterable).each(func);
    timer.elapsed();
}

template<typename Func>
void pathological_with(Func func) {
    entt::registry registry;
    auto view = func(registry);

    for(std::uint64_t i = 0; i < 500000L; i++) {
        const auto entity = registry.create();
        registry.emplace<position>(entity);
        registry.emplace<velocity>(entity);
        registry.emplace<comp<0>>(entity);
    }

    for(auto i = 0; i < 10; ++i) {
        registry.each([i = 0, &registry](const auto entity) mutable {
            if(!(++i % 7)) {
                registry.remove<position>(entity);
            }

            if(!(++i % 11)) {
                registry.remove<velocity>(entity);
            }

            if(!(++i % 13)) {
                registry.remove<comp<0>>(entity);
            }

            if(!(++i % 17)) {
                registry.destroy(entity);
            }
        });

        for(std::uint64_t j = 0; j < 50000L; j++) {
            const auto entity = registry.create();
            registry.emplace<position>(entity);
            registry.emplace<velocity>(entity);
            registry.emplace<comp<0>>(entity);
        }
    }

    timer timer;
    view.each([](auto &...comp) { ((comp.x = {}), ...); });
    timer.elapsed();
}

TEST(Benchmark, Create) {
    entt::registry registry;

    std::cout << "Creating 1000000 entities" << std::endl;

    generic_with([&]() {
        for(std::uint64_t i = 0; i < 1000000L; i++) {
            static_cast<void>(registry.create());
        }
    });
}

TEST(Benchmark, CreateMany) {
    entt::registry registry;
    std::vector<entt::entity> entities(1000000);

    std::cout << "Creating 1000000 entities at once" << std::endl;

    generic_with([&]() {
        registry.create(entities.begin(), entities.end());
    });
}

TEST(Benchmark, CreateManyAndEmplaceComponents) {
    entt::registry registry;
    std::vector<entt::entity> entities(1000000);

    std::cout << "Creating 1000000 entities at once and emplace components" << std::endl;

    generic_with([&]() {
        registry.create(entities.begin(), entities.end());

        for(const auto entity: entities) {
            registry.emplace<position>(entity);
            registry.emplace<velocity>(entity);
        }
    });
}

TEST(Benchmark, CreateManyWithComponents) {
    entt::registry registry;
    std::vector<entt::entity> entities(1000000);

    std::cout << "Creating 1000000 entities at once with components" << std::endl;

    generic_with([&]() {
        registry.create(entities.begin(), entities.end());
        registry.insert<position>(entities.begin(), entities.end());
        registry.insert<velocity>(entities.begin(), entities.end());
    });
}

TEST(Benchmark, Erase) {
    entt::registry registry;
    std::vector<entt::entity> entities(1000000);
    auto view = registry.view<position>();

    std::cout << "Erasing 1000000 components from their entities" << std::endl;

    registry.create(entities.begin(), entities.end());
    registry.insert<position>(entities.begin(), entities.end());

    generic_with([&]() {
        for(auto entity: view) {
            registry.erase<position>(entity);
        }
    });
}

TEST(Benchmark, EraseMany) {
    entt::registry registry;
    std::vector<entt::entity> entities(1000000);
    auto view = registry.view<position>();

    std::cout << "Erasing 1000000 components from their entities at once" << std::endl;

    registry.create(entities.begin(), entities.end());
    registry.insert<position>(entities.begin(), entities.end());

    generic_with([&]() {
        registry.erase<position>(view.begin(), view.end());
    });
}

TEST(Benchmark, EraseManyMulti) {
    entt::registry registry;
    std::vector<entt::entity> entities(1000000);
    auto view = registry.view<position>();

    std::cout << "Erasing 1000000 components per type from their entities at once" << std::endl;

    registry.create(entities.begin(), entities.end());
    registry.insert<position>(entities.begin(), entities.end());
    registry.insert<velocity>(entities.begin(), entities.end());

    generic_with([&]() {
        registry.erase<position, velocity>(view.begin(), view.end());
    });
}

TEST(Benchmark, Remove) {
    entt::registry registry;
    std::vector<entt::entity> entities(1000000);
    auto view = registry.view<position>();

    std::cout << "Removing 1000000 components from their entities" << std::endl;

    registry.create(entities.begin(), entities.end());
    registry.insert<position>(entities.begin(), entities.end());

    generic_with([&]() {
        for(auto entity: view) {
            registry.remove<position>(entity);
        }
    });
}

TEST(Benchmark, RemoveMany) {
    entt::registry registry;
    std::vector<entt::entity> entities(1000000);
    auto view = registry.view<position>();

    std::cout << "Removing 1000000 components from their entities at once" << std::endl;

    registry.create(entities.begin(), entities.end());
    registry.insert<position>(entities.begin(), entities.end());

    generic_with([&]() {
        registry.remove<position>(view.begin(), view.end());
    });
}

TEST(Benchmark, RemoveManyMulti) {
    entt::registry registry;
    std::vector<entt::entity> entities(1000000);
    auto view = registry.view<position>();

    std::cout << "Removing 1000000 components per type from their entities at once" << std::endl;

    registry.create(entities.begin(), entities.end());
    registry.insert<position>(entities.begin(), entities.end());
    registry.insert<velocity>(entities.begin(), entities.end());

    generic_with([&]() {
        registry.remove<position, velocity>(view.begin(), view.end());
    });
}

TEST(Benchmark, Clear) {
    entt::registry registry;
    std::vector<entt::entity> entities(1000000);

    std::cout << "Clearing 1000000 components from their entities" << std::endl;

    registry.create(entities.begin(), entities.end());
    registry.insert<position>(entities.begin(), entities.end());

    generic_with([&]() {
        registry.clear<position>();
    });
}

TEST(Benchmark, ClearMulti) {
    entt::registry registry;
    std::vector<entt::entity> entities(1000000);

    std::cout << "Clearing 1000000 components per type from their entities" << std::endl;

    registry.create(entities.begin(), entities.end());
    registry.insert<position>(entities.begin(), entities.end());
    registry.insert<velocity>(entities.begin(), entities.end());

    generic_with([&]() {
        registry.clear<position, velocity>();
    });
}

TEST(Benchmark, ClearStable) {
    entt::registry registry;
    std::vector<entt::entity> entities(1000000);

    std::cout << "Clearing 1000000 stable components from their entities" << std::endl;

    registry.create(entities.begin(), entities.end());
    registry.insert<stable_position>(entities.begin(), entities.end());

    generic_with([&]() {
        registry.clear<stable_position>();
    });
}

TEST(Benchmark, Recycle) {
    entt::registry registry;
    std::vector<entt::entity> entities(1000000);

    std::cout << "Recycling 1000000 entities" << std::endl;

    registry.create(entities.begin(), entities.end());
    registry.destroy(entities.begin(), entities.end());

    generic_with([&]() {
        for(auto next = entities.size(); next; --next) {
            entities[next] = registry.create();
        }
    });
}

TEST(Benchmark, RecycleMany) {
    entt::registry registry;
    std::vector<entt::entity> entities(1000000);

    std::cout << "Recycling 1000000 entities" << std::endl;

    registry.create(entities.begin(), entities.end());
    registry.destroy(entities.begin(), entities.end());

    generic_with([&]() {
        registry.create(entities.begin(), entities.end());
    });
}

TEST(Benchmark, Destroy) {
    entt::registry registry;
    std::vector<entt::entity> entities(1000000);
    auto view = registry.view<position>();

    std::cout << "Destroying 1000000 entities" << std::endl;

    registry.create(entities.begin(), entities.end());
    registry.insert<position>(entities.begin(), entities.end());

    generic_with([&]() {
        for(auto entity: view) {
            registry.destroy(entity);
        }
    });
}

TEST(Benchmark, DestroyMany) {
    entt::registry registry;
    std::vector<entt::entity> entities(1000000);
    auto view = registry.view<position>();

    std::cout << "Destroying 1000000 entities at once" << std::endl;

    registry.create(entities.begin(), entities.end());
    registry.insert<position>(entities.begin(), entities.end());

    generic_with([&]() {
        registry.destroy(view.begin(), view.end());
    });
}

TEST(Benchmark, DestroyManyMulti) {
    entt::registry registry;
    std::vector<entt::entity> entities(1000000);
    auto view = registry.view<position>();

    std::cout << "Destroying 1000000 entities at once, multiple components" << std::endl;

    registry.create(entities.begin(), entities.end());
    registry.insert<position>(entities.begin(), entities.end());
    registry.insert<velocity>(entities.begin(), entities.end());

    generic_with([&]() {
        registry.destroy(view.begin(), view.end());
    });
}

TEST(Benchmark, GetFromRegistry) {
    entt::registry registry;
    std::vector<entt::entity> entities(1000000);

    registry.create(entities.begin(), entities.end());
    registry.insert<position>(entities.begin(), entities.end());

    generic_with([&]() {
        for(auto entity: entities) {
            registry.get<position>(entity).x = 0u;
        }
    });
}

TEST(Benchmark, GetFromRegistryMulti) {
    entt::registry registry;
    std::vector<entt::entity> entities(1000000);

    registry.create(entities.begin(), entities.end());
    registry.insert<position>(entities.begin(), entities.end());
    registry.insert<velocity>(entities.begin(), entities.end());

    generic_with([&]() {
        for(auto entity: entities) {
            registry.get<position>(entity).x = 0u;
            registry.get<velocity>(entity).y = 0u;
        }
    });
}

TEST(Benchmark, GetFromView) {
    entt::registry registry;
    std::vector<entt::entity> entities(1000000);
    auto view = registry.view<position>();

    registry.create(entities.begin(), entities.end());
    registry.insert<position>(entities.begin(), entities.end());

    generic_with([&]() {
        for(auto entity: entities) {
            view.get<position>(entity).x = 0u;
        }
    });
}

TEST(Benchmark, GetFromViewMulti) {
    entt::registry registry;
    std::vector<entt::entity> entities(1000000);
    auto view = registry.view<position, velocity>();

    registry.create(entities.begin(), entities.end());
    registry.insert<position>(entities.begin(), entities.end());
    registry.insert<velocity>(entities.begin(), entities.end());

    generic_with([&]() {
        for(auto entity: entities) {
            view.get<position>(entity).x = 0u;
            view.get<velocity>(entity).y = 0u;
        }
    });
}

TEST(Benchmark, IterateSingleComponent1M) {
    entt::registry registry;

    std::cout << "Iterating over 1000000 entities, one component" << std::endl;

    for(std::uint64_t i = 0; i < 1000000L; i++) {
        const auto entity = registry.create();
        registry.emplace<position>(entity);
    }

    iterate_with(registry.view<position>(), [](auto &...comp) {
        ((comp.x = {}), ...);
    });
}

TEST(Benchmark, IterateSingleStableComponent1M) {
    entt::registry registry;

    std::cout << "Iterating over 1000000 entities, one stable component" << std::endl;

    for(std::uint64_t i = 0; i < 1000000L; i++) {
        const auto entity = registry.create();
        registry.emplace<stable_position>(entity);
    }

    iterate_with(registry.view<stable_position>(), [](auto &...comp) {
        ((comp.x = {}), ...);
    });
}

TEST(Benchmark, IterateSingleComponentRuntime1M) {
    entt::registry registry;

    std::cout << "Iterating over 1000000 entities, one component, runtime view" << std::endl;

    for(std::uint64_t i = 0; i < 1000000L; i++) {
        const auto entity = registry.create();
        registry.emplace<position>(entity);
    }

    entt::runtime_view view{};
    view.iterate(registry.storage<position>());

    iterate_with(view, [&](auto entity) {
        registry.get<position>(entity).x = {};
    });
}

TEST(Benchmark, IterateTwoComponents1M) {
    entt::registry registry;

    std::cout << "Iterating over 1000000 entities, two components" << std::endl;

    for(std::uint64_t i = 0; i < 1000000L; i++) {
        const auto entity = registry.create();
        registry.emplace<position>(entity);
        registry.emplace<velocity>(entity);
    }

    iterate_with(registry.view<position, velocity>(), [](auto &...comp) {
        ((comp.x = {}), ...);
    });
}

TEST(Benchmark, IterateTwoStableComponents1M) {
    entt::registry registry;

    std::cout << "Iterating over 1000000 entities, two stable components" << std::endl;

    for(std::uint64_t i = 0; i < 1000000L; i++) {
        const auto entity = registry.create();
        registry.emplace<stable_position>(entity);
        registry.emplace<velocity>(entity);
    }

    iterate_with(registry.view<stable_position, velocity>(), [](auto &...comp) {
        ((comp.x = {}), ...);
    });
}

TEST(Benchmark, IterateTwoComponents1MHalf) {
    entt::registry registry;

    std::cout << "Iterating over 1000000 entities, two components, half of the entities have all the components" << std::endl;

    for(std::uint64_t i = 0; i < 1000000L; i++) {
        const auto entity = registry.create();
        registry.emplace<velocity>(entity);

        if(i % 2) {
            registry.emplace<position>(entity);
        }
    }

    iterate_with(registry.view<position, velocity>(), [](auto &...comp) {
        ((comp.x = {}), ...);
    });
}

TEST(Benchmark, IterateTwoComponents1MOne) {
    entt::registry registry;

    std::cout << "Iterating over 1000000 entities, two components, only one entity has all the components" << std::endl;

    for(std::uint64_t i = 0; i < 1000000L; i++) {
        const auto entity = registry.create();
        registry.emplace<velocity>(entity);

        if(i == 500000L) {
            registry.emplace<position>(entity);
        }
    }

    iterate_with(registry.view<position, velocity>(), [](auto &...comp) {
        ((comp.x = {}), ...);
    });
}

TEST(Benchmark, IterateTwoComponentsNonOwningGroup1M) {
    entt::registry registry;

    std::cout << "Iterating over 1000000 entities, two components, non owning group" << std::endl;

    for(std::uint64_t i = 0; i < 1000000L; i++) {
        const auto entity = registry.create();
        registry.emplace<position>(entity);
        registry.emplace<velocity>(entity);
    }

    iterate_with(registry.group<>(entt::get<position, velocity>), [](auto &...comp) {
        ((comp.x = {}), ...);
    });
}

TEST(Benchmark, IterateTwoComponentsFullOwningGroup1M) {
    entt::registry registry;

    std::cout << "Iterating over 1000000 entities, two components, full owning group" << std::endl;

    for(std::uint64_t i = 0; i < 1000000L; i++) {
        const auto entity = registry.create();
        registry.emplace<position>(entity);
        registry.emplace<velocity>(entity);
    }

    iterate_with(registry.group<position, velocity>(), [](auto &...comp) {
        ((comp.x = {}), ...);
    });
}

TEST(Benchmark, IterateTwoComponentsPartialOwningGroup1M) {
    entt::registry registry;

    std::cout << "Iterating over 1000000 entities, two components, partial owning group" << std::endl;

    for(std::uint64_t i = 0; i < 1000000L; i++) {
        const auto entity = registry.create();
        registry.emplace<position>(entity);
        registry.emplace<velocity>(entity);
    }

    iterate_with(registry.group<position>(entt::get<velocity>), [](auto &...comp) {
        ((comp.x = {}), ...);
    });
}

TEST(Benchmark, IterateTwoComponentsRuntime1M) {
    entt::registry registry;

    std::cout << "Iterating over 1000000 entities, two components, runtime view" << std::endl;

    for(std::uint64_t i = 0; i < 1000000L; i++) {
        const auto entity = registry.create();
        registry.emplace<position>(entity);
        registry.emplace<velocity>(entity);
    }

    entt::runtime_view view{};
    view.iterate(registry.storage<position>())
        .iterate(registry.storage<velocity>());

    iterate_with(view, [&](auto entity) {
        registry.get<position>(entity).x = {};
        registry.get<velocity>(entity).x = {};
    });
}

TEST(Benchmark, IterateTwoComponentsRuntime1MHalf) {
    entt::registry registry;

    std::cout << "Iterating over 1000000 entities, two components, half of the entities have all the components, runtime view" << std::endl;

    for(std::uint64_t i = 0; i < 1000000L; i++) {
        const auto entity = registry.create();
        registry.emplace<velocity>(entity);

        if(i % 2) {
            registry.emplace<position>(entity);
        }
    }

    entt::runtime_view view{};
    view.iterate(registry.storage<position>())
        .iterate(registry.storage<velocity>());

    iterate_with(view, [&](auto entity) {
        registry.get<position>(entity).x = {};
        registry.get<velocity>(entity).x = {};
    });
}

TEST(Benchmark, IterateTwoComponentsRuntime1MOne) {
    entt::registry registry;

    std::cout << "Iterating over 1000000 entities, two components, only one entity has all the components, runtime view" << std::endl;

    for(std::uint64_t i = 0; i < 1000000L; i++) {
        const auto entity = registry.create();
        registry.emplace<velocity>(entity);

        if(i == 500000L) {
            registry.emplace<position>(entity);
        }
    }

    entt::runtime_view view{};
    view.iterate(registry.storage<position>())
        .iterate(registry.storage<velocity>());

    iterate_with(view, [&](auto entity) {
        registry.get<position>(entity).x = {};
        registry.get<velocity>(entity).x = {};
    });
}

TEST(Benchmark, IterateThreeComponents1M) {
    entt::registry registry;

    std::cout << "Iterating over 1000000 entities, three components" << std::endl;

    for(std::uint64_t i = 0; i < 1000000L; i++) {
        const auto entity = registry.create();
        registry.emplace<position>(entity);
        registry.emplace<velocity>(entity);
        registry.emplace<comp<0>>(entity);
    }

    iterate_with(registry.view<position, velocity, comp<0>>(), [](auto &...comp) {
        ((comp.x = {}), ...);
    });
}

TEST(Benchmark, IterateThreeStableComponents1M) {
    entt::registry registry;

    std::cout << "Iterating over 1000000 entities, three stable components" << std::endl;

    for(std::uint64_t i = 0; i < 1000000L; i++) {
        const auto entity = registry.create();
        registry.emplace<stable_position>(entity);
        registry.emplace<velocity>(entity);
        registry.emplace<comp<0>>(entity);
    }

    iterate_with(registry.view<stable_position, velocity, comp<0>>(), [](auto &...comp) {
        ((comp.x = {}), ...);
    });
}

TEST(Benchmark, IterateThreeComponents1MHalf) {
    entt::registry registry;

    std::cout << "Iterating over 1000000 entities, three components, half of the entities have all the components" << std::endl;

    for(std::uint64_t i = 0; i < 1000000L; i++) {
        const auto entity = registry.create();
        registry.emplace<velocity>(entity);
        registry.emplace<comp<0>>(entity);

        if(i % 2) {
            registry.emplace<position>(entity);
        }
    }

    iterate_with(registry.view<position, velocity, comp<0>>(), [](auto &...comp) {
        ((comp.x = {}), ...);
    });
}

TEST(Benchmark, IterateThreeComponents1MOne) {
    entt::registry registry;

    std::cout << "Iterating over 1000000 entities, three components, only one entity has all the components" << std::endl;

    for(std::uint64_t i = 0; i < 1000000L; i++) {
        const auto entity = registry.create();
        registry.emplace<velocity>(entity);
        registry.emplace<comp<0>>(entity);

        if(i == 500000L) {
            registry.emplace<position>(entity);
        }
    }

    iterate_with(registry.view<position, velocity, comp<0>>(), [](auto &...comp) {
        ((comp.x = {}), ...);
    });
}

TEST(Benchmark, IterateThreeComponentsNonOwningGroup1M) {
    entt::registry registry;

    std::cout << "Iterating over 1000000 entities, three components, non owning group" << std::endl;

    for(std::uint64_t i = 0; i < 1000000L; i++) {
        const auto entity = registry.create();
        registry.emplace<position>(entity);
        registry.emplace<velocity>(entity);
        registry.emplace<comp<0>>(entity);
    }

    iterate_with(registry.group<>(entt::get<position, velocity, comp<0>>), [](auto &...comp) {
        ((comp.x = {}), ...);
    });
}

TEST(Benchmark, IterateThreeComponentsFullOwningGroup1M) {
    entt::registry registry;

    std::cout << "Iterating over 1000000 entities, three components, full owning group" << std::endl;

    for(std::uint64_t i = 0; i < 1000000L; i++) {
        const auto entity = registry.create();
        registry.emplace<position>(entity);
        registry.emplace<velocity>(entity);
        registry.emplace<comp<0>>(entity);
    }

    iterate_with(registry.group<position, velocity, comp<0>>(), [](auto &...comp) {
        ((comp.x = {}), ...);
    });
}

TEST(Benchmark, IterateThreeComponentsPartialOwningGroup1M) {
    entt::registry registry;

    std::cout << "Iterating over 1000000 entities, three components, partial owning group" << std::endl;

    for(std::uint64_t i = 0; i < 1000000L; i++) {
        const auto entity = registry.create();
        registry.emplace<position>(entity);
        registry.emplace<velocity>(entity);
        registry.emplace<comp<0>>(entity);
    }

    iterate_with(registry.group<position, velocity>(entt::get<comp<0>>), [](auto &...comp) {
        ((comp.x = {}), ...);
    });
}

TEST(Benchmark, IterateThreeComponentsRuntime1M) {
    entt::registry registry;

    std::cout << "Iterating over 1000000 entities, three components, runtime view" << std::endl;

    for(std::uint64_t i = 0; i < 1000000L; i++) {
        const auto entity = registry.create();
        registry.emplace<position>(entity);
        registry.emplace<velocity>(entity);
        registry.emplace<comp<0>>(entity);
    }

    entt::runtime_view view{};
    view.iterate(registry.storage<position>())
        .iterate(registry.storage<velocity>())
        .iterate(registry.storage<comp<0>>());

    iterate_with(view, [&](auto entity) {
        registry.get<position>(entity).x = {};
        registry.get<velocity>(entity).x = {};
        registry.get<comp<0>>(entity).x = {};
    });
}

TEST(Benchmark, IterateThreeComponentsRuntime1MHalf) {
    entt::registry registry;

    std::cout << "Iterating over 1000000 entities, three components, half of the entities have all the components, runtime view" << std::endl;

    for(std::uint64_t i = 0; i < 1000000L; i++) {
        const auto entity = registry.create();
        registry.emplace<velocity>(entity);
        registry.emplace<comp<0>>(entity);

        if(i % 2) {
            registry.emplace<position>(entity);
        }
    }

    entt::runtime_view view{};
    view.iterate(registry.storage<position>())
        .iterate(registry.storage<velocity>())
        .iterate(registry.storage<comp<0>>());

    iterate_with(view, [&](auto entity) {
        registry.get<position>(entity).x = {};
        registry.get<velocity>(entity).x = {};
        registry.get<comp<0>>(entity).x = {};
    });
}

TEST(Benchmark, IterateThreeComponentsRuntime1MOne) {
    entt::registry registry;

    std::cout << "Iterating over 1000000 entities, three components, only one entity has all the components, runtime view" << std::endl;

    for(std::uint64_t i = 0; i < 1000000L; i++) {
        const auto entity = registry.create();
        registry.emplace<velocity>(entity);
        registry.emplace<comp<0>>(entity);

        if(i == 500000L) {
            registry.emplace<position>(entity);
        }
    }

    entt::runtime_view view{};
    view.iterate(registry.storage<position>())
        .iterate(registry.storage<velocity>())
        .iterate(registry.storage<comp<0>>());

    iterate_with(view, [&](auto entity) {
        registry.get<position>(entity).x = {};
        registry.get<velocity>(entity).x = {};
        registry.get<comp<0>>(entity).x = {};
    });
}

TEST(Benchmark, IterateFiveComponents1M) {
    entt::registry registry;

    std::cout << "Iterating over 1000000 entities, five components" << std::endl;

    for(std::uint64_t i = 0; i < 1000000L; i++) {
        const auto entity = registry.create();
        registry.emplace<position>(entity);
        registry.emplace<velocity>(entity);
        registry.emplace<comp<0>>(entity);
        registry.emplace<comp<1>>(entity);
        registry.emplace<comp<2>>(entity);
    }

    iterate_with(registry.view<position, velocity, comp<0>, comp<1>, comp<2>>(), [](auto &...comp) {
        ((comp.x = {}), ...);
    });
}

TEST(Benchmark, IterateFiveStableComponents1M) {
    entt::registry registry;

    std::cout << "Iterating over 1000000 entities, five stable components" << std::endl;

    for(std::uint64_t i = 0; i < 1000000L; i++) {
        const auto entity = registry.create();
        registry.emplace<stable_position>(entity);
        registry.emplace<velocity>(entity);
        registry.emplace<comp<0>>(entity);
        registry.emplace<comp<1>>(entity);
        registry.emplace<comp<2>>(entity);
    }

    iterate_with(registry.view<stable_position, velocity, comp<0>, comp<1>, comp<2>>(), [](auto &...comp) {
        ((comp.x = {}), ...);
    });
}

TEST(Benchmark, IterateFiveComponents1MHalf) {
    entt::registry registry;

    std::cout << "Iterating over 1000000 entities, five components, half of the entities have all the components" << std::endl;

    for(std::uint64_t i = 0; i < 1000000L; i++) {
        const auto entity = registry.create();
        registry.emplace<velocity>(entity);
        registry.emplace<comp<0>>(entity);
        registry.emplace<comp<1>>(entity);
        registry.emplace<comp<2>>(entity);

        if(i % 2) {
            registry.emplace<position>(entity);
        }
    }

    iterate_with(registry.view<position, velocity, comp<0>, comp<1>, comp<2>>(), [](auto &...comp) {
        ((comp.x = {}), ...);
    });
}

TEST(Benchmark, IterateFiveComponents1MOne) {
    entt::registry registry;

    std::cout << "Iterating over 1000000 entities, five components, only one entity has all the components" << std::endl;

    for(std::uint64_t i = 0; i < 1000000L; i++) {
        const auto entity = registry.create();
        registry.emplace<velocity>(entity);
        registry.emplace<comp<0>>(entity);
        registry.emplace<comp<1>>(entity);
        registry.emplace<comp<2>>(entity);

        if(i == 500000L) {
            registry.emplace<position>(entity);
        }
    }

    iterate_with(registry.view<position, velocity, comp<0>, comp<1>, comp<2>>(), [](auto &...comp) {
        ((comp.x = {}), ...);
    });
}

TEST(Benchmark, IterateFiveComponentsNonOwningGroup1M) {
    entt::registry registry;

    std::cout << "Iterating over 1000000 entities, five components, non owning group" << std::endl;

    for(std::uint64_t i = 0; i < 1000000L; i++) {
        const auto entity = registry.create();
        registry.emplace<position>(entity);
        registry.emplace<velocity>(entity);
        registry.emplace<comp<0>>(entity);
        registry.emplace<comp<1>>(entity);
        registry.emplace<comp<2>>(entity);
    }

    iterate_with(registry.group<>(entt::get<position, velocity, comp<0>, comp<1>, comp<2>>), [](auto &...comp) {
        ((comp.x = {}), ...);
    });
}

TEST(Benchmark, IterateFiveComponentsFullOwningGroup1M) {
    entt::registry registry;

    std::cout << "Iterating over 1000000 entities, five components, full owning group" << std::endl;

    for(std::uint64_t i = 0; i < 1000000L; i++) {
        const auto entity = registry.create();
        registry.emplace<position>(entity);
        registry.emplace<velocity>(entity);
        registry.emplace<comp<0>>(entity);
        registry.emplace<comp<1>>(entity);
        registry.emplace<comp<2>>(entity);
    }

    iterate_with(registry.group<position, velocity, comp<0>, comp<1>, comp<2>>(), [](auto &...comp) {
        ((comp.x = {}), ...);
    });
}

TEST(Benchmark, IterateFiveComponentsPartialFourOfFiveOwningGroup1M) {
    entt::registry registry;

    std::cout << "Iterating over 1000000 entities, five components, partial (4 of 5) owning group" << std::endl;

    for(std::uint64_t i = 0; i < 1000000L; i++) {
        const auto entity = registry.create();
        registry.emplace<position>(entity);
        registry.emplace<velocity>(entity);
        registry.emplace<comp<0>>(entity);
        registry.emplace<comp<1>>(entity);
        registry.emplace<comp<2>>(entity);
    }

    iterate_with(registry.group<position, velocity, comp<0>, comp<1>>(entt::get<comp<2>>), [](auto &...comp) {
        ((comp.x = {}), ...);
    });
}

TEST(Benchmark, IterateFiveComponentsPartialThreeOfFiveOwningGroup1M) {
    entt::registry registry;

    std::cout << "Iterating over 1000000 entities, five components, partial (3 of 5) owning group" << std::endl;

    for(std::uint64_t i = 0; i < 1000000L; i++) {
        const auto entity = registry.create();
        registry.emplace<position>(entity);
        registry.emplace<velocity>(entity);
        registry.emplace<comp<0>>(entity);
        registry.emplace<comp<1>>(entity);
        registry.emplace<comp<2>>(entity);
    }

    iterate_with(registry.group<position, velocity, comp<0>>(entt::get<comp<1>, comp<2>>), [](auto &...comp) {
        ((comp.x = {}), ...);
    });
}

TEST(Benchmark, IterateFiveComponentsRuntime1M) {
    entt::registry registry;

    std::cout << "Iterating over 1000000 entities, five components, runtime view" << std::endl;

    for(std::uint64_t i = 0; i < 1000000L; i++) {
        const auto entity = registry.create();
        registry.emplace<position>(entity);
        registry.emplace<velocity>(entity);
        registry.emplace<comp<0>>(entity);
        registry.emplace<comp<1>>(entity);
        registry.emplace<comp<2>>(entity);
    }

    entt::runtime_view view{};
    view.iterate(registry.storage<position>())
        .iterate(registry.storage<velocity>())
        .iterate(registry.storage<comp<0>>())
        .iterate(registry.storage<comp<1>>())
        .iterate(registry.storage<comp<2>>());

    iterate_with(view, [&](auto entity) {
        registry.get<position>(entity).x = {};
        registry.get<velocity>(entity).x = {};
        registry.get<comp<0>>(entity).x = {};
        registry.get<comp<1>>(entity).x = {};
        registry.get<comp<2>>(entity).x = {};
    });
}

TEST(Benchmark, IterateFiveComponentsRuntime1MHalf) {
    entt::registry registry;

    std::cout << "Iterating over 1000000 entities, five components, half of the entities have all the components, runtime view" << std::endl;

    for(std::uint64_t i = 0; i < 1000000L; i++) {
        const auto entity = registry.create();
        registry.emplace<velocity>(entity);
        registry.emplace<comp<0>>(entity);
        registry.emplace<comp<1>>(entity);
        registry.emplace<comp<2>>(entity);

        if(i % 2) {
            registry.emplace<position>(entity);
        }
    }

    entt::runtime_view view{};
    view.iterate(registry.storage<position>())
        .iterate(registry.storage<velocity>())
        .iterate(registry.storage<comp<0>>())
        .iterate(registry.storage<comp<1>>())
        .iterate(registry.storage<comp<2>>());

    iterate_with(view, [&](auto entity) {
        registry.get<position>(entity).x = {};
        registry.get<velocity>(entity).x = {};
        registry.get<comp<0>>(entity).x = {};
        registry.get<comp<1>>(entity).x = {};
        registry.get<comp<2>>(entity).x = {};
    });
}

TEST(Benchmark, IterateFiveComponentsRuntime1MOne) {
    entt::registry registry;

    std::cout << "Iterating over 1000000 entities, five components, only one entity has all the components, runtime view" << std::endl;

    for(std::uint64_t i = 0; i < 1000000L; i++) {
        const auto entity = registry.create();
        registry.emplace<velocity>(entity);
        registry.emplace<comp<0>>(entity);
        registry.emplace<comp<1>>(entity);
        registry.emplace<comp<2>>(entity);

        if(i == 500000L) {
            registry.emplace<position>(entity);
        }
    }

    entt::runtime_view view{};
    view.iterate(registry.storage<position>())
        .iterate(registry.storage<velocity>())
        .iterate(registry.storage<comp<0>>())
        .iterate(registry.storage<comp<1>>())
        .iterate(registry.storage<comp<2>>());

    iterate_with(view, [&](auto entity) {
        registry.get<position>(entity).x = {};
        registry.get<velocity>(entity).x = {};
        registry.get<comp<0>>(entity).x = {};
        registry.get<comp<1>>(entity).x = {};
        registry.get<comp<2>>(entity).x = {};
    });
}

TEST(Benchmark, IteratePathological) {
    std::cout << "Pathological case" << std::endl;
    pathological_with([](auto &registry) { return registry.template view<position, velocity, comp<0>>(); });
}

TEST(Benchmark, IteratePathologicalNonOwningGroup) {
    std::cout << "Pathological case (non-owning group)" << std::endl;
    pathological_with([](auto &registry) { return registry.template group<>(entt::get<position, velocity, comp<0>>); });
}

TEST(Benchmark, IteratePathologicalFullOwningGroup) {
    std::cout << "Pathological case (full-owning group)" << std::endl;
    pathological_with([](auto &registry) { return registry.template group<position, velocity, comp<0>>(); });
}

TEST(Benchmark, IteratePathologicalPartialOwningGroup) {
    std::cout << "Pathological case (partial-owning group)" << std::endl;
    pathological_with([](auto &registry) { return registry.template group<position, velocity>(entt::get<comp<0>>); });
}

TEST(Benchmark, SortSingle) {
    entt::registry registry;

    std::cout << "Sort 150000 entities, one component" << std::endl;

    for(std::uint64_t i = 0; i < 150000L; i++) {
        const auto entity = registry.create();
        registry.emplace<position>(entity, i, i);
    }

    generic_with([&]() {
        registry.sort<position>([](const auto &lhs, const auto &rhs) { return lhs.x < rhs.x && lhs.y < rhs.y; });
    });
}

TEST(Benchmark, SortMulti) {
    entt::registry registry;

    std::cout << "Sort 150000 entities, two components" << std::endl;

    for(std::uint64_t i = 0; i < 150000L; i++) {
        const auto entity = registry.create();
        registry.emplace<position>(entity, i, i);
        registry.emplace<velocity>(entity, i, i);
    }

    registry.sort<position>([](const auto &lhs, const auto &rhs) { return lhs.x < rhs.x && lhs.y < rhs.y; });

    generic_with([&]() {
        registry.sort<velocity, position>();
    });
}

TEST(Benchmark, AlmostSortedStdSort) {
    entt::registry registry;
    entt::entity entities[3]{};

    std::cout << "Sort 150000 entities, almost sorted, std::sort" << std::endl;

    for(std::uint64_t i = 0; i < 150000L; i++) {
        const auto entity = registry.create();
        registry.emplace<position>(entity, i, i);

        if(!(i % 50000)) {
            entities[i / 50000] = entity;
        }
    }

    for(std::uint64_t i = 0; i < 3; ++i) {
        registry.destroy(entities[i]);
        const auto entity = registry.create();
        registry.emplace<position>(entity, 50000 * i, 50000 * i);
    }

    generic_with([&]() {
        registry.sort<position>([](const auto &lhs, const auto &rhs) { return lhs.x > rhs.x && lhs.y > rhs.y; });
    });
}

TEST(Benchmark, AlmostSortedInsertionSort) {
    entt::registry registry;
    entt::entity entities[3]{};

    std::cout << "Sort 150000 entities, almost sorted, insertion sort" << std::endl;

    for(std::uint64_t i = 0; i < 150000L; i++) {
        const auto entity = registry.create();
        registry.emplace<position>(entity, i, i);

        if(!(i % 50000)) {
            entities[i / 50000] = entity;
        }
    }

    for(std::uint64_t i = 0; i < 3; ++i) {
        registry.destroy(entities[i]);
        const auto entity = registry.create();
        registry.emplace<position>(entity, 50000 * i, 50000 * i);
    }

    generic_with([&]() {
        registry.sort<position>([](const auto &lhs, const auto &rhs) { return lhs.x > rhs.x && lhs.y > rhs.y; }, entt::insertion_sort{});
    });
}
