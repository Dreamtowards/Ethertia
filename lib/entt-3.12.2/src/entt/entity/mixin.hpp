#ifndef ENTT_ENTITY_MIXIN_HPP
#define ENTT_ENTITY_MIXIN_HPP

#include <type_traits>
#include <utility>
#include "../config/config.h"
#include "../core/any.hpp"
#include "../signal/sigh.hpp"
#include "entity.hpp"
#include "fwd.hpp"

namespace entt {

/**
 * @brief Mixin type used to add signal support to storage types.
 *
 * The function type of a listener is equivalent to:
 *
 * @code{.cpp}
 * void(basic_registry<entity_type> &, entity_type);
 * @endcode
 *
 * This applies to all signals made available.
 *
 * @tparam Type The type of the underlying storage.
 */
template<typename Type>
class sigh_mixin final: public Type {
    using underlying_type = Type;
    using basic_registry_type = basic_registry<typename underlying_type::entity_type, typename underlying_type::base_type::allocator_type>;
    using sigh_type = sigh<void(basic_registry_type &, const typename underlying_type::entity_type), typename underlying_type::allocator_type>;
    using underlying_iterator = typename underlying_type::base_type::basic_iterator;

    basic_registry_type &owner_or_assert() const noexcept {
        ENTT_ASSERT(owner != nullptr, "Invalid pointer to registry");
        return *owner;
    }

    void pop(underlying_iterator first, underlying_iterator last) final {
        if(auto &reg = owner_or_assert(); destruction.empty()) {
            underlying_type::pop(first, last);
        } else {
            for(; first != last; ++first) {
                const auto entt = *first;
                destruction.publish(reg, entt);
                const auto it = underlying_type::find(entt);
                underlying_type::pop(it, it + 1u);
            }
        }
    }

    void pop_all() final {
        if(auto &reg = owner_or_assert(); !destruction.empty()) {
            for(auto pos = underlying_type::each().begin().base().index(); !(pos < 0); --pos) {
                if constexpr(underlying_type::traits_type::in_place_delete) {
                    if(const auto entt = underlying_type::operator[](static_cast<typename underlying_type::size_type>(pos)); entt != tombstone) {
                        destruction.publish(reg, entt);
                    }
                } else {
                    destruction.publish(reg, underlying_type::operator[](static_cast<typename underlying_type::size_type>(pos)));
                }
            }
        }

        underlying_type::pop_all();
    }

    underlying_iterator try_emplace(const typename underlying_type::entity_type entt, const bool force_back, const void *value) final {
        const auto it = underlying_type::try_emplace(entt, force_back, value);

        if(auto &reg = owner_or_assert(); it != underlying_type::base_type::end()) {
            construction.publish(reg, *it);
        }

        return it;
    }

public:
    /*! @brief Allocator type. */
    using allocator_type = typename underlying_type::allocator_type;
    /*! @brief Underlying entity identifier. */
    using entity_type = typename underlying_type::entity_type;
    /*! @brief Expected registry type. */
    using registry_type = basic_registry_type;

    /*! @brief Default constructor. */
    sigh_mixin()
        : sigh_mixin{allocator_type{}} {}

    /**
     * @brief Constructs an empty storage with a given allocator.
     * @param allocator The allocator to use.
     */
    explicit sigh_mixin(const allocator_type &allocator)
        : underlying_type{allocator},
          owner{},
          construction{allocator},
          destruction{allocator},
          update{allocator} {}

    /**
     * @brief Move constructor.
     * @param other The instance to move from.
     */
    sigh_mixin(sigh_mixin &&other) noexcept
        : underlying_type{std::move(other)},
          owner{other.owner},
          construction{std::move(other.construction)},
          destruction{std::move(other.destruction)},
          update{std::move(other.update)} {}

    /**
     * @brief Allocator-extended move constructor.
     * @param other The instance to move from.
     * @param allocator The allocator to use.
     */
    sigh_mixin(sigh_mixin &&other, const allocator_type &allocator) noexcept
        : underlying_type{std::move(other), allocator},
          owner{other.owner},
          construction{std::move(other.construction), allocator},
          destruction{std::move(other.destruction), allocator},
          update{std::move(other.update), allocator} {}

    /**
     * @brief Move assignment operator.
     * @param other The instance to move from.
     * @return This storage.
     */
    sigh_mixin &operator=(sigh_mixin &&other) noexcept {
        underlying_type::operator=(std::move(other));
        owner = other.owner;
        construction = std::move(other.construction);
        destruction = std::move(other.destruction);
        update = std::move(other.update);
        return *this;
    }

    /**
     * @brief Exchanges the contents with those of a given storage.
     * @param other Storage to exchange the content with.
     */
    void swap(sigh_mixin &other) {
        using std::swap;
        underlying_type::swap(other);
        swap(owner, other.owner);
        swap(construction, other.construction);
        swap(destruction, other.destruction);
        swap(update, other.update);
    }

    /**
     * @brief Returns a sink object.
     *
     * The sink returned by this function can be used to receive notifications
     * whenever a new instance is created and assigned to an entity.<br/>
     * Listeners are invoked after the object has been assigned to the entity.
     *
     * @sa sink
     *
     * @return A temporary sink object.
     */
    [[nodiscard]] auto on_construct() noexcept {
        return sink{construction};
    }

    /**
     * @brief Returns a sink object.
     *
     * The sink returned by this function can be used to receive notifications
     * whenever an instance is explicitly updated.<br/>
     * Listeners are invoked after the object has been updated.
     *
     * @sa sink
     *
     * @return A temporary sink object.
     */
    [[nodiscard]] auto on_update() noexcept {
        return sink{update};
    }

    /**
     * @brief Returns a sink object.
     *
     * The sink returned by this function can be used to receive notifications
     * whenever an instance is removed from an entity and thus destroyed.<br/>
     * Listeners are invoked before the object has been removed from the entity.
     *
     * @sa sink
     *
     * @return A temporary sink object.
     */
    [[nodiscard]] auto on_destroy() noexcept {
        return sink{destruction};
    }

    /**
     * @brief Emplace elements into a storage.
     *
     * The behavior of this operation depends on the underlying storage type
     * (for example, components vs entities).<br/>
     * Refer to the specific documentation for more details.
     *
     * @return A return value as returned by the underlying storage.
     */
    auto emplace() {
        const auto entt = underlying_type::emplace();
        construction.publish(owner_or_assert(), entt);
        return entt;
    }

    /**
     * @brief Emplace elements into a storage.
     *
     * The behavior of this operation depends on the underlying storage type
     * (for example, components vs entities).<br/>
     * Refer to the specific documentation for more details.
     *
     * @tparam Args Types of arguments to forward to the underlying storage.
     * @param hint A valid identifier.
     * @param args Parameters to forward to the underlying storage.
     * @return A return value as returned by the underlying storage.
     */
    template<typename... Args>
    decltype(auto) emplace(const entity_type hint, Args &&...args) {
        if constexpr(std::is_same_v<typename underlying_type::value_type, typename underlying_type::entity_type>) {
            const auto entt = underlying_type::emplace(hint, std::forward<Args>(args)...);
            construction.publish(owner_or_assert(), entt);
            return entt;
        } else {
            underlying_type::emplace(hint, std::forward<Args>(args)...);
            construction.publish(owner_or_assert(), hint);
            return this->get(hint);
        }
    }

    /**
     * @brief Patches the given instance for an entity.
     * @tparam Func Types of the function objects to invoke.
     * @param entt A valid identifier.
     * @param func Valid function objects.
     * @return A reference to the patched instance.
     */
    template<typename... Func>
    decltype(auto) patch(const entity_type entt, Func &&...func) {
        underlying_type::patch(entt, std::forward<Func>(func)...);
        update.publish(owner_or_assert(), entt);
        return this->get(entt);
    }

    /**
     * @brief Emplace elements into a storage.
     *
     * The behavior of this operation depends on the underlying storage type
     * (for example, components vs entities).<br/>
     * Refer to the specific documentation for more details.
     *
     * @tparam It Iterator type (as required by the underlying storage type).
     * @tparam Args Types of arguments to forward to the underlying storage.
     * @param first An iterator to the first element of the range.
     * @param last An iterator past the last element of the range.
     * @param args Parameters to use to forward to the underlying storage.
     */
    template<typename It, typename... Args>
    void insert(It first, It last, Args &&...args) {
        underlying_type::insert(first, last, std::forward<Args>(args)...);

        if(auto &reg = owner_or_assert(); !construction.empty()) {
            for(; first != last; ++first) {
                construction.publish(reg, *first);
            }
        }
    }

    /**
     * @brief Forwards variables to derived classes, if any.
     * @param value A variable wrapped in an opaque container.
     */
    void bind(any value) noexcept final {
        auto *reg = any_cast<basic_registry_type>(&value);
        owner = reg ? reg : owner;
        underlying_type::bind(std::move(value));
    }

private:
    basic_registry_type *owner;
    sigh_type construction;
    sigh_type destruction;
    sigh_type update;
};

} // namespace entt

#endif
