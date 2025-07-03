#include "BackSimpleForm.h"
#include "ll/api/form/SimpleForm.h"


namespace ltps {


BackSimpleForm::BackSimpleForm() = default;
BackSimpleForm::BackSimpleForm(BackCallback backCallback, ButtonPos buttonPos)
: mButtonPos(buttonPos),
  mBackCallback(std::move(backCallback)) {
    if (mBackCallback && !mIsAddedBackButton) {
        if (mButtonPos == ButtonPos::Upper) {
            mIsAddedBackButton = true;
            ll::form::SimpleForm::appendButton("Back", "textures/ui/icon_import", "path", mBackCallback);
        }
    }
}

BackSimpleForm& BackSimpleForm::sendTo(Player& player, Callback callback) {
    if (mBackCallback && !mIsAddedBackButton) {
        if (mButtonPos == ButtonPos::Lower) {
            mIsAddedBackButton = true;
            appendButton("Back", "textures/ui/icon_import", "path", mBackCallback);
        }
    }
    ll::form::SimpleForm::sendTo(player, std::move(callback));
    return *this;
}


} // namespace ltps