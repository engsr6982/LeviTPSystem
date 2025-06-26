#pragma once
#include "ll/api/form/SimpleForm.h"
#include "ltps/Global.h"
#include <functional>
#include <tuple>
#include <type_traits>

namespace ltps {

class BackSimpleForm : public ll::form::SimpleForm {
public:
    enum class ButtonPos { Upper, Lower };

    using Callback       = ll::form::SimpleForm::Callback;
    using ButtonCallback = ll::form::SimpleForm::ButtonCallback;
    using BackCallback   = std::function<void(Player&)>;

    TPSAPI explicit BackSimpleForm();
    TPSAPI explicit BackSimpleForm(BackCallback backCallback, ButtonPos buttonPos = ButtonPos::Upper);

    BackSimpleForm(const BackSimpleForm&)            = delete;
    BackSimpleForm& operator=(const BackSimpleForm&) = delete;

    // hiding methods
    TPSAPI BackSimpleForm& sendTo(Player& player, Callback callback = Callback());


    // factory method
    template <auto Fn, typename... Args>
    static BackCallback makeCallback(Args&&... args) {
        static_assert(
            std::is_invocable_v<decltype(Fn), Player&, Args...>,
            "Fn must be callable with (Player&, Args...)"
        );

        return [args = std::make_tuple(std::forward<Args>(args)...)](Player& p) mutable {
            std::apply(
                [&p](auto&&... unpacked) {
                    Fn(p, std::forward<decltype(unpacked)>(unpacked)...); // 直接调用
                },
                std::move(args)
            );
        };
    }


    template <auto ParentFn = nullptr, auto BP = ButtonPos::Upper, typename... Args>
    static BackSimpleForm make(Args&&... args) {
        if constexpr (ParentFn == nullptr) {
            return BackSimpleForm{}; // 没有父表单，不需要返回按钮
        } else {
            return BackSimpleForm{makeCallback<ParentFn>(std::forward<Args>(args)...), BP};
        }
    }


private:
    bool         mIsAddedBackButton{false};
    ButtonPos    mButtonPos{ButtonPos::Upper};
    BackCallback mBackCallback{nullptr};
};

} // namespace ltps
