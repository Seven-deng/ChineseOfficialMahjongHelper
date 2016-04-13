﻿#include "PointsCalculatorScene.h"
#include "../common.h"
#include "../mahjong-algorithm/points_calculator.h"

#include "../widget/TilePickWidget.h"

USING_NS_CC;

Scene *PointsCalculatorScene::createScene() {
    auto scene = Scene::create();
    auto layer = PointsCalculatorScene::create();
    scene->addChild(layer);
    return scene;
}

bool PointsCalculatorScene::init() {
    if (!BaseLayer::initWithTitle("国标麻将算番器")) {
        return false;
    }

    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    _tilePicker = TilePickWidget::create();
    const Size &widgetSize = _tilePicker->getContentSize();
    this->addChild(_tilePicker);
    _tilePicker->setFixedSetsChangedCallback(std::bind(&PointsCalculatorScene::onFixedSetsChanged, this, std::placeholders::_1));
    _tilePicker->setWinTileChangedCallback(std::bind(&PointsCalculatorScene::onWinTileChanged, this, std::placeholders::_1));

    float y = origin.y + visibleSize.height - 10;
    if (widgetSize.width > visibleSize.width) {
        float scale = visibleSize.width / widgetSize.width;
        _tilePicker->setScale(scale);
        y -= widgetSize.height * scale;
        _tilePicker->setPosition(Vec2(origin.x + visibleSize.width * 0.5f, y - 20 + widgetSize.height * scale * 0.5f));
    }
    else {
        y -= widgetSize.height;
        _tilePicker->setPosition(Vec2(origin.x + widgetSize.width * 0.5f, y - 20 + widgetSize.height * 0.5f));
    }

    ui::Widget *infoWidget = ui::Widget::create();
    infoWidget->setContentSize(Size(visibleSize.width, 120));
    this->addChild(infoWidget);
    infoWidget->setPosition(Vec2(origin.x + visibleSize.width * 0.5f, y - 80));

    _pointsAreaNode = Node::create();
    _pointsAreaNode->setContentSize(Size(visibleSize.width, y - 140));
    _pointsAreaNode->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    this->addChild(_pointsAreaNode);
    _pointsAreaNode->setPosition(Vec2(origin.x + visibleSize.width * 0.5f, origin.y + y * 0.5f - 70));

    float gapX = (visibleSize.width - 8) * 0.25f;
    _byDiscardButton = ui::Button::create("source_material/btn_square_normal.png", "source_material/btn_square_highlighted.png");
    infoWidget->addChild(_byDiscardButton);
    _byDiscardButton->setScale9Enabled(true);
    _byDiscardButton->setContentSize(Size(20.0f, 20.0f));
    _byDiscardButton->setPosition(Vec2(20.0f, 105.0f));
    setButtonChecked(_byDiscardButton);
    _byDiscardButton->addClickEventListener(std::bind(&PointsCalculatorScene::onByDiscardButton, this, std::placeholders::_1));

    Label *byDiscardLabel = Label::createWithSystemFont("点和", "Arial", 12);
    infoWidget->addChild(byDiscardLabel);
    byDiscardLabel->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
    byDiscardLabel->setPosition(Vec2(35.0f, 105.0f));

    _selfDrawnButton = ui::Button::create("source_material/btn_square_normal.png", "source_material/btn_square_highlighted.png");
    infoWidget->addChild(_selfDrawnButton);
    _selfDrawnButton->setScale9Enabled(true);
    _selfDrawnButton->setContentSize(Size(20.0f, 20.0f));
    _selfDrawnButton->setPosition(Vec2(20.0f + gapX, 105.0f));
    setButtonUnchecked(_selfDrawnButton);
    _selfDrawnButton->addClickEventListener(std::bind(&PointsCalculatorScene::onSelfDrawnButton, this, std::placeholders::_1));

    Label *selfDrawnLabel = Label::createWithSystemFont("自摸", "Arial", 12);
    infoWidget->addChild(selfDrawnLabel);
    selfDrawnLabel->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
    selfDrawnLabel->setPosition(Vec2(35.0f + gapX, 105.0f));

    _fourthTileButton = ui::Button::create("source_material/btn_square_normal.png", "source_material/btn_square_highlighted.png", "source_material/btn_square_disabled.png");
    infoWidget->addChild(_fourthTileButton);
    _fourthTileButton->setScale9Enabled(true);
    _fourthTileButton->setContentSize(Size(20.0f, 20.0f));
    _fourthTileButton->setPosition(Vec2(20.0f + gapX * 2, 105.0f));
    setButtonUnchecked(_fourthTileButton);
    _fourthTileButton->setEnabled(false);
    _fourthTileButton->addClickEventListener(std::bind(&PointsCalculatorScene::onFourthTileButton, this, std::placeholders::_1));

    Label *fourthTileLabel = Label::createWithSystemFont("绝张", "Arial", 12);
    infoWidget->addChild(fourthTileLabel);
    fourthTileLabel->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
    fourthTileLabel->setPosition(Vec2(35.0f + gapX * 2, 105.0f));

    _replacementButton = ui::Button::create("source_material/btn_square_normal.png", "source_material/btn_square_highlighted.png", "source_material/btn_square_disabled.png");
    infoWidget->addChild(_replacementButton);
    _replacementButton->setScale9Enabled(true);
    _replacementButton->setContentSize(Size(20.0f, 20.0f));
    _replacementButton->setPosition(Vec2(20.0f, 75.0f));
    setButtonUnchecked(_replacementButton);
    _replacementButton->setEnabled(false);
    _replacementButton->addClickEventListener(std::bind(&PointsCalculatorScene::onReplacementButton, this, std::placeholders::_1));

    Label *replacementLabel = Label::createWithSystemFont("杠开", "Arial", 12);
    infoWidget->addChild(replacementLabel);
    replacementLabel->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
    replacementLabel->setPosition(Vec2(35.0f, 75.0f));

    _robKongButton = ui::Button::create("source_material/btn_square_normal.png", "source_material/btn_square_highlighted.png", "source_material/btn_square_disabled.png");
    infoWidget->addChild(_robKongButton);
    _robKongButton->setScale9Enabled(true);
    _robKongButton->setContentSize(Size(20.0f, 20.0f));
    _robKongButton->setPosition(Vec2(20.0f + gapX, 75.0f));
    setButtonUnchecked(_robKongButton);
    _robKongButton->setEnabled(false);
    _robKongButton->addClickEventListener(std::bind(&PointsCalculatorScene::onRobKongButton, this, std::placeholders::_1));

    Label *robKongLabel = Label::createWithSystemFont("抢杠", "Arial", 12);
    infoWidget->addChild(robKongLabel);
    robKongLabel->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
    robKongLabel->setPosition(Vec2(35.0f + gapX, 75.0f));

    _lastTileButton = ui::Button::create("source_material/btn_square_normal.png", "source_material/btn_square_highlighted.png", "source_material/btn_square_disabled.png");
    infoWidget->addChild(_lastTileButton);
    _lastTileButton->setScale9Enabled(true);
    _lastTileButton->setContentSize(Size(20.0f, 20.0f));
    _lastTileButton->setPosition(Vec2(20.0f + gapX * 2, 75.0f));
    setButtonUnchecked(_lastTileButton);
    _lastTileButton->addClickEventListener(std::bind(&PointsCalculatorScene::onLastTileButton, this, std::placeholders::_1));

    Label *lastTileDrawnLabel = Label::createWithSystemFont("海底", "Arial", 12);
    infoWidget->addChild(lastTileDrawnLabel);
    lastTileDrawnLabel->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
    lastTileDrawnLabel->setPosition(Vec2(35.0f + gapX * 2, 75.0f));

    const char *windName[4] = { "东", "南", "西", "北" };

    Label *prevalentWindLabel = Label::createWithSystemFont("圈风", "Arial", 12);
    infoWidget->addChild(prevalentWindLabel);
    prevalentWindLabel->setPosition(Vec2(20.0f, 45.0f));

    for (int i = 0; i < 4; ++i) {
        _prevalentButton[i] = ui::Button::create("source_material/btn_square_normal.png", "", "source_material/btn_square_highlighted.png");
        infoWidget->addChild(_prevalentButton[i]);
        _prevalentButton[i]->setScale9Enabled(true);
        _prevalentButton[i]->setContentSize(Size(20.0f, 20.0f));
        _prevalentButton[i]->setTitleColor(Color3B::BLACK);
        _prevalentButton[i]->setTitleFontSize(12);
        _prevalentButton[i]->setTitleText(windName[i]);
        _prevalentButton[i]->setPosition(Vec2(50.0f + i * 30, 45.0f));
        _prevalentButton[i]->addClickEventListener([this, i](Ref *) {
            for (int n = 0; n < 4; ++n) {
                _prevalentButton[n]->setEnabled(n != i);
            }
        });
    }
    _prevalentButton[0]->setEnabled(false);

    Label *seatWindLabel = Label::createWithSystemFont("门风", "Arial", 12);
    infoWidget->addChild(seatWindLabel);
    seatWindLabel->setPosition(Vec2(20.0f, 15.0f));

    for (int i = 0; i < 4; ++i) {
        _seatButton[i] = ui::Button::create("source_material/btn_square_normal.png", "", "source_material/btn_square_highlighted.png");
        infoWidget->addChild(_seatButton[i]);
        _seatButton[i]->setScale9Enabled(true);
        _seatButton[i]->setContentSize(Size(20.0f, 20.0f));
        _seatButton[i]->setTitleColor(Color3B::BLACK);
        _seatButton[i]->setTitleFontSize(12);
        _seatButton[i]->setTitleText(windName[i]);
        _seatButton[i]->setPosition(Vec2(50.0f + i * 30, 15.0f));
        _seatButton[i]->addClickEventListener([this, i](Ref *) {
            for (int n = 0; n < 4; ++n) {
                _seatButton[n]->setEnabled(n != i);
            }
        });
    }
    _seatButton[0]->setEnabled(false);

    Label *flowerLabel = Label::createWithSystemFont("花牌数", "Arial", 12);
    infoWidget->addChild(flowerLabel);
    flowerLabel->setAnchorPoint(Vec2::ANCHOR_MIDDLE_RIGHT);
    flowerLabel->setPosition(Vec2(visibleSize.width - 50, 45.0f));

    _editBox = ui::EditBox::create(Size(35.0f, 20.0f), ui::Scale9Sprite::create("source_material/tabbar_background1.png"));
    infoWidget->addChild(_editBox);
    _editBox->setInputFlag(ui::EditBox::InputFlag::SENSITIVE);
    _editBox->setInputMode(ui::EditBox::InputMode::NUMERIC);
    _editBox->setFontColor(Color4B::BLACK);
    _editBox->setFontSize(12);
    _editBox->setText("0");
    _editBox->setPosition(Vec2(visibleSize.width - 30, 45.0f));

    ui::Button *button = ui::Button::create("source_material/btn_square_normal.png", "source_material/btn_square_selected.png", "source_material/btn_square_disabled.png");
    button->setScale9Enabled(true);
    button->setContentSize(Size(35.0f, 20.0f));
    button->setTitleFontSize(12);
    button->setTitleText("算番");
    button->setTitleColor(Color3B::BLACK);
    infoWidget->addChild(button);
    button->setPosition(Vec2(visibleSize.width - 30, 15.0f));
    button->addClickEventListener([this](Ref *) { calculate(); });

    return true;
}

void PointsCalculatorScene::onByDiscardButton(cocos2d::Ref *sender) {
    if (!isButtonChecked(_byDiscardButton)) {
        setButtonChecked(_byDiscardButton);
        setButtonUnchecked(_selfDrawnButton);

        setButtonUnchecked(_replacementButton);
        _replacementButton->setEnabled(false);

        setButtonUnchecked(_fourthTileButton);
        _fourthTileButton->setEnabled(_maybeFourthTile);

        setButtonUnchecked(_robKongButton);
        _robKongButton->setEnabled(_maybeFourthTile);
    }
    else {
        setButtonChecked(_byDiscardButton);
    }
}

void PointsCalculatorScene::onSelfDrawnButton(cocos2d::Ref *sender) {
    if (!isButtonChecked(_selfDrawnButton)) {
        setButtonChecked(_selfDrawnButton);
        setButtonUnchecked(_byDiscardButton);

        setButtonUnchecked(_robKongButton);
        _robKongButton->setEnabled(false);

        setButtonUnchecked(_replacementButton);
        _replacementButton->setEnabled(_hasKong);
    }
    else {
        setButtonChecked(_selfDrawnButton);
    }
}

void PointsCalculatorScene::onFourthTileButton(cocos2d::Ref *sender) {
    if (isButtonChecked(_fourthTileButton)) {
        setButtonUnchecked(_fourthTileButton);
        setButtonUnchecked(_robKongButton);
        _robKongButton->setEnabled(isButtonChecked(_byDiscardButton));
    }
    else {
        setButtonChecked(_fourthTileButton);
        setButtonUnchecked(_robKongButton);
        _robKongButton->setEnabled(false);
    }
}

void PointsCalculatorScene::onRobKongButton(cocos2d::Ref *sender) {
    if (isButtonChecked(_robKongButton)) {
        setButtonUnchecked(_robKongButton);
        setButtonUnchecked(_fourthTileButton);
        _fourthTileButton->setEnabled(isButtonChecked(_byDiscardButton));
    }
    else {
        setButtonChecked(_robKongButton);
        setButtonUnchecked(_fourthTileButton);
        _fourthTileButton->setEnabled(false);
    }
}

void PointsCalculatorScene::onReplacementButton(cocos2d::Ref *sender) {
    if (isButtonChecked(_replacementButton)) {
        setButtonUnchecked(_replacementButton);
    }
    else {
        setButtonChecked(_replacementButton);
    }
}

void PointsCalculatorScene::onLastTileButton(cocos2d::Ref *sender) {
    if (isButtonChecked(_lastTileButton)) {
        setButtonUnchecked(_lastTileButton);
    }
    else {
        setButtonChecked(_lastTileButton);
    }
}

void PointsCalculatorScene::onFixedSetsChanged(TilePickWidget *sender) {
    bool prevValue = _hasKong;
    const std::vector<mahjong::SET> &fixedSets = sender->getFixedSets();
    auto it = std::find_if(fixedSets.begin(), fixedSets.end(), [](const mahjong::SET &s) { return s.set_type == mahjong::SET_TYPE::KONG; });
    _hasKong = it != fixedSets.end();
    if (prevValue != _hasKong) {
        if (isButtonChecked(_selfDrawnButton)) {
            _replacementButton->setEnabled(_hasKong);
        }
    }
}

void PointsCalculatorScene::onWinTileChanged(TilePickWidget *sender) {
    bool prevValue = _maybeFourthTile;
    mahjong::TILE winTile = sender->getWinTile();
    if (winTile != 0) {
        _maybeFourthTile = true;
        const std::vector<mahjong::TILE> &handTiles = sender->getHandTiles();
        if (handTiles.end() != std::find(handTiles.begin(), handTiles.end(), winTile)) {
            _maybeFourthTile = false;
        }
        if (_maybeFourthTile) {
            const std::vector<mahjong::SET> &fixedSets = sender->getFixedSets();
            if (fixedSets.end()
                != std::find_if(fixedSets.begin(), fixedSets.end(), std::bind(&mahjong::is_set_contains_tile, std::placeholders::_1, winTile))) {
                _maybeFourthTile = false;
            }
        }
    }
    else {
        _maybeFourthTile = false;
    }

    if (_maybeFourthTile != prevValue) {
        _fourthTileButton->setEnabled(true);
        if (isButtonChecked(_byDiscardButton)) {
            _robKongButton->setEnabled(true);
        }
    }
}

#define FONT_SIZE 14

void PointsCalculatorScene::calculate() {
    _pointsAreaNode->removeAllChildren();

    Size visibleSize = Director::getInstance()->getVisibleSize();
    const Size &pointsAreaSize = _pointsAreaNode->getContentSize();
    Vec2 pos(pointsAreaSize.width * 0.5f, pointsAreaSize.height * 0.5f);

    std::string errorStr;
    do {
        int flowerCnt = atoi(_editBox->getText());
        if (flowerCnt > 8) {
            errorStr = "花牌数不得超过8";
            break;
        }

        mahjong::SET sets[5];
        long set_cnt = std::copy(_tilePicker->getFixedSets().begin(), _tilePicker->getFixedSets().end(), std::begin(sets))
            - std::begin(sets);

        mahjong::TILE tiles[13];
        long tile_cnt = std::copy(_tilePicker->getHandTiles().begin(), _tilePicker->getHandTiles().end(), std::begin(tiles))
            - std::begin(tiles);
        mahjong::sort_tiles(tiles, tile_cnt);

        mahjong::TILE win_tile = _tilePicker->getWinTile();

        long points_table[mahjong::POINT_TYPE_COUNT] = { 0 };
        mahjong::WIN_TYPE win_type = WIN_TYPE_DISCARD;
        if (_selfDrawnButton->isHighlighted()) win_type |= WIN_TYPE_SELF_DRAWN;
        if (_fourthTileButton->isHighlighted()) win_type |= WIN_TYPE_4TH_TILE;
        if (_robKongButton->isHighlighted()) win_type |= WIN_TYPE_ABOUT_KONG;
        if (_replacementButton->isHighlighted()) win_type |= (WIN_TYPE_ABOUT_KONG | WIN_TYPE_SELF_DRAWN);
        if (_lastTileButton->isHighlighted()) win_type |= WIN_TYPE_WALL_LAST;

        mahjong::WIND_TYPE prevalent_wind = mahjong::WIND_TYPE::EAST, seat_wind = mahjong::WIND_TYPE::EAST;
        for (int i = 0; i < 4; ++i) {
            if (!_prevalentButton[i]->isEnabled()) {
                prevalent_wind = static_cast<mahjong::WIND_TYPE>(static_cast<int>(mahjong::WIND_TYPE::EAST) + i);
                break;
            }
        }
        for (int i = 0; i < 4; ++i) {
            if (!_seatButton[i]->isEnabled()) {
                seat_wind = static_cast<mahjong::WIND_TYPE>(static_cast<int>(mahjong::WIND_TYPE::EAST) + i);
                break;
            }
        }

        int points = calculate_points(sets, set_cnt, tiles, tile_cnt, win_tile, win_type, prevalent_wind, seat_wind, points_table);
        if (points == ERROR_NOT_WIN) {
            errorStr = "诈和";
            break;
        }
        if (points == ERROR_WRONG_TILES_COUNT) {
            errorStr = "相公";
            break;
        }

        points += flowerCnt;
        points_table[mahjong::FLOWER_TILES] = flowerCnt;
        long n = mahjong::POINT_TYPE_COUNT - std::count(std::begin(points_table), std::end(points_table), 0);
        long rows = n / 2 + n % 2;
        Node *innerNode = Node::create();
        float pointsAreaHeight = (FONT_SIZE + 2) * (rows + 2);
        innerNode->setContentSize(Size(visibleSize.width, pointsAreaHeight));

        for (int i = 0, j = 0; i < n; ++i) {
            while (points_table[++j] == 0) continue;

            std::string str;
            if (points_table[j] == 1) {
                str = StringUtils::format("%s %d\n", mahjong::points_name[j], mahjong::points_value_table[j]);
            }
            else {
                str = StringUtils::format("%s %d*%ld\n", mahjong::points_name[j], mahjong::points_value_table[j], points_table[j]);
            }

            Label *pointName = Label::createWithSystemFont(str, "Arial", FONT_SIZE);
            innerNode->addChild(pointName);
            pointName->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
            div_t ret = div(i, 2);
            pointName->setPosition(Vec2(ret.rem == 0 ? 5.0f : visibleSize.width * 0.5f + 5.0f, (FONT_SIZE + 2) * (rows - ret.quot + 2)));
        }
        Label *pointTotal = Label::createWithSystemFont(StringUtils::format("总计：%d番", points), "Arial", FONT_SIZE);
        innerNode->addChild(pointTotal);
        pointTotal->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
        pointTotal->setPosition(Vec2(5.0f, FONT_SIZE + 2));

        if (pointsAreaHeight <= pointsAreaSize.height) {
            _pointsAreaNode->addChild(innerNode);
            innerNode->setAnchorPoint(Vec2(0.5f, 0.5f));
            innerNode->setPosition(pos);
        }
        else {
            ui::ScrollView *scrollView = ui::ScrollView::create();
            scrollView->setDirection(ui::ScrollView::Direction::VERTICAL);
            scrollView->setScrollBarPositionFromCorner(Vec2(10, 10));
            scrollView->setContentSize(pointsAreaSize);
            scrollView->setInnerContainerSize(innerNode->getContentSize());
            scrollView->addChild(innerNode);

            _pointsAreaNode->addChild(scrollView);
            scrollView->setAnchorPoint(Vec2(0.5f, 0.5f));
            scrollView->setPosition(pos);
        }

        return;

    } while (0);

    Label *errorLabel = Label::createWithSystemFont(errorStr, "Arial", FONT_SIZE);
    _pointsAreaNode->addChild(errorLabel);
    errorLabel->setPosition(pos);
}
