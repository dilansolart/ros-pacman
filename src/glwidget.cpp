#include "pacman/glwidget.h"

GLWidget::GLWidget(QWidget *parent)
{
    firstTime = true;
    allowToPlay = false;
    nPacman = 0;
    nGhosts = 0;
    ghostModeTimer = new QTimer();
    frightenedGhostModeTimer = new QTimer();
    ghostModeTimer->setSingleShot(true);
    frightenedGhostModeTimer->setSingleShot(true);
    connect(ghostModeTimer, SIGNAL(timeout()), this, SLOT(ToggleGhostModeSlot()));
    connect(frightenedGhostModeTimer, SIGNAL(timeout()), this, SLOT(EndOfFrightenedGhostModeSlot()));
    contGhostModePhases = 0;
    ghostRemainingTime = 0;
    isInFrightenedMode = false;
}

GLWidget::~GLWidget()
{

}

QSize GLWidget::minimumSizeHint() const
{
    return QSize(50, 50);
}

QSize GLWidget::sizeHint() const
{
    return QSize(_mapWidth, _mapHeight);
}

void GLWidget::LoadTexture (QImage* img)
{
    GLuint tex;
    glEnable(GL_TEXTURE_2D); // Enable texturing
    QImage t = (img->convertToFormat(QImage::Format_RGBA8888)).mirrored();
    glGenTextures(1, &tex); // Obtain an id for the texture
    glBindTexture(GL_TEXTURE_2D, tex); // Set as the current texture
    texIds.append(tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, t.width(), t.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, t.bits());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glDisable(GL_TEXTURE_2D);
}

void GLWidget::LoadNewTexture (QImage* img)
{
    GLuint tex;
    glEnable(GL_TEXTURE_2D); // Enable texturing
    QImage t = (img->convertToFormat(QImage::Format_RGBA8888)).mirrored();
    glGenTextures(1, &tex); // Obtain an id for the texture
    glBindTexture(GL_TEXTURE_2D, tex); // Set as the current texture
    texIds[18] = tex;	
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, t.width(), t.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, t.bits());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glDisable(GL_TEXTURE_2D);
}

void GLWidget::DrawMap()
{
    glColor3f(0.7, 0.7, 0.7);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texIds[18]);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f);
    glVertex2f  (-0.5*_mapWidth, -0.5*_mapHeight);
    glTexCoord2f(0.0f,  1.0f);
    glVertex2f  (-0.5*_mapWidth,  0.5*_mapHeight);
    glTexCoord2f(1.0f,  1.0f);
    glVertex2f  (0.5*_mapWidth,  0.5*_mapHeight);
    glTexCoord2f(1.0f, 0.0f);
    glVertex2f  (0.5*_mapWidth, -0.5*_mapHeight);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

void GLWidget::DrawPacman()
{
    for(int i=0;i < nPacman;i++)
    {
	glShadeModel(GL_SMOOTH);
	glPushMatrix();
	glTranslated(pacmanArray[i]->currentPosition.x(),pacmanArray[i]->currentPosition.y(), 0);
	glRotated(pacmanArray[i]->orientation, 0, 0, 1);
	glColor3f(1.0, 1.0, 1.0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texIds[0]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex2f  (-0.5*pacmanArray[i]->width, -0.5*pacmanArray[i]->height);
	glTexCoord2f(0.0f,  1.0f);
	glVertex2f  (-0.5*pacmanArray[i]->width,  0.5*pacmanArray[i]->height);
	glTexCoord2f(1.0f,  1.0f);
	glVertex2f  (0.5*pacmanArray[i]->width,  0.5*pacmanArray[i]->height);
	glTexCoord2f(1.0f, 0.0f);
	glVertex2f  (0.5*pacmanArray[i]->width, -0.5*pacmanArray[i]->height);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
	glColor3f(1.0, 1.0, 1.0);
    }
}

void GLWidget::DrawGhosts()
{
    for(int i=0;i < nGhosts;i++)
    {
	glShadeModel(GL_SMOOTH);
	glPushMatrix();
	glTranslated(ghostsArray[i]->currentPosition.x(), ghostsArray[i]->currentPosition.y(), 0);
	glRotated(0, 0, 0, 1);
	glColor3f(1.0, 1.0, 1.0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texIds[ghostsArray[i]->GetTexId()]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex2f  (-0.5*ghostsArray[i]->width , -0.5*ghostsArray[i]->height);
	glTexCoord2f(0.0f,  1.0f);
	glVertex2f  (-0.5*ghostsArray[i]->width,  0.5*ghostsArray[i]->height);
	glTexCoord2f(1.0f,  1.0f);
	glVertex2f  (0.5*ghostsArray[i]->width,  0.5*ghostsArray[i]->height);
	glTexCoord2f(1.0f, 0.0f);
	glVertex2f  (0.5*ghostsArray[i]->width, -0.5*ghostsArray[i]->height);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
	glColor3f(1.0, 1.0, 1.0);
    }
}

void GLWidget::DrawCookies()
{
    for(int i = 0; i < cookiesCoord->size(); i++)
	DrawCircle(cookiesCoord->at(i).x(), cookiesCoord->at(i).y(), 6.0, 1.0, 1.0, 0.0);
}

void GLWidget::DrawBonus()
{
    for(int i = 0; i < bonusCoord->size(); i++)
	DrawCircle(bonusCoord->at(i).x(), bonusCoord->at(i).y(), 11.0, 1.0, 0.8, 0.0);
}

void GLWidget::DrawCircle(float x, float y, float radius, float red, float green, float blue) 
{ 
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glTranslatef(x, y, 0.0f);
    glColor3f(red, green, blue);
    static const int circle_points = 100;
    static const float angle = 2.0f * 3.1416f / circle_points;

    // this code (mostly) copied from question:
    glBegin(GL_POLYGON);
    double angle1=0.0;
    glVertex2d(radius * cos(0.0) , radius * sin(0.0));
    int i;
    for (i=0; i<circle_points; i++)
    {       
        glVertex2d(radius * cos(angle1), radius *sin(angle1));
        angle1 += angle;
    }
    glEnd();
    glPopMatrix();
}

void GLWidget::TogglePlaying()
{
    if (allowToPlay)
    {
      allowToPlay = false;
      ghostModeTimer->stop();
      contGhostModePhases = 0;
    }
    else
    {
      ghostModeTimer->start(ghostModeTimes[contGhostModePhases]);
      contGhostModePhases++;
      allowToPlay = true;
    }
}

void GLWidget::UpdatePacmanPosition(int i)
{
    int stepX = pacmanArray[i]->width, stepY = pacmanArray[i]->height;
    
    if(pacmanArray[i]->action == Pacman::Action::Right)
    {
      QPoint coord(pacmanArray[i]->currentPosition.x() + (int)(pacmanArray[i]->width*0.5)  + 1, pacmanArray[i]->currentPosition.y());
      if (!_obstacles[utilities.GetIndexRowFromCoord(coord, _mapHeight)*_mapWidth + utilities.GetIndexColFromCoord(coord, _mapWidth)])
	  pacmanArray[i]->currentPosition.setX(pacmanArray[i]->currentPosition.x() + stepX);
      pacmanArray[i]->orientation = 0.0;
    }
    else if(pacmanArray[i]->action == Pacman::Action::Left)
    {
      QPoint coord(pacmanArray[i]->currentPosition.x() - (int)(pacmanArray[i]->width*0.5) - 1, pacmanArray[i]->currentPosition.y());
      if (!_obstacles[utilities.GetIndexRowFromCoord(coord, _mapHeight)*_mapWidth + utilities.GetIndexColFromCoord(coord, _mapWidth)])
	  pacmanArray[i]->currentPosition.setX(pacmanArray[i]->currentPosition.x() - stepX);
      pacmanArray[i]->orientation = 180.0;
    }
    else if(pacmanArray[i]->action == Pacman::Action::Up)
    {
      QPoint coord(pacmanArray[i]->currentPosition.x(), pacmanArray[i]->currentPosition.y() + (int)(pacmanArray[i]->height*0.5) + 1);
      if (!_obstacles[utilities.GetIndexRowFromCoord(coord, _mapHeight)*_mapWidth + utilities.GetIndexColFromCoord(coord, _mapWidth)])
	  pacmanArray[i]->currentPosition.setY(pacmanArray[i]->currentPosition.y() + stepY);
      pacmanArray[i]->orientation = 90.0;
    }
    else if(pacmanArray[i]->action == Pacman::Action::Down)
    {
      QPoint coord(pacmanArray[i]->currentPosition.x(), pacmanArray[i]->currentPosition.y() - (int)(pacmanArray[i]->height*0.5) - 1);
      if (!_obstacles[utilities.GetIndexRowFromCoord(coord, _mapHeight)*_mapWidth + utilities.GetIndexColFromCoord(coord, _mapWidth)])
	  pacmanArray[i]->currentPosition.setY(pacmanArray[i]->currentPosition.y() - stepY);
      pacmanArray[i]->orientation = 270.0;
    }
    pacmanArray[i]->action = Pacman::Action::None;    
}

void GLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    LoadTexture(new QImage(tr(":/resources/textures/pacman.jpeg")));    
    LoadTexture(new QImage(tr(":/resources/textures/redGhostUp.png")));
    LoadTexture(new QImage(tr(":/resources/textures/redGhostDown.png")));
    LoadTexture(new QImage(tr(":/resources/textures/redGhostRight.png")));
    LoadTexture(new QImage(tr(":/resources/textures/redGhostLeft.png")));
    LoadTexture(new QImage(tr(":/resources/textures/pinkGhostUp.png")));
    LoadTexture(new QImage(tr(":/resources/textures/pinkGhostDown.png")));
    LoadTexture(new QImage(tr(":/resources/textures/pinkGhostRight.png")));
    LoadTexture(new QImage(tr(":/resources/textures/pinkGhostLeft.png")));
    LoadTexture(new QImage(tr(":/resources/textures/blueGhostUp.png")));
    LoadTexture(new QImage(tr(":/resources/textures/blueGhostDown.png")));
    LoadTexture(new QImage(tr(":/resources/textures/blueGhostRight.png")));
    LoadTexture(new QImage(tr(":/resources/textures/blueGhostLeft.png")));
    LoadTexture(new QImage(tr(":/resources/textures/orangeGhostUp.png")));
    LoadTexture(new QImage(tr(":/resources/textures/orangeGhostDown.png")));
    LoadTexture(new QImage(tr(":/resources/textures/orangeGhostRight.png")));
    LoadTexture(new QImage(tr(":/resources/textures/orangeGhostLeft.png")));
    LoadTexture(new QImage(tr(":/resources/textures/frightenedGhost.png")));
    LoadTexture(_mapImage);
}

void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(ortho[0], ortho[1], ortho[2], ortho[3], -10, 10);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
      
    DrawMap();
    DrawCookies();
    DrawBonus();
    DrawPacman();
    DrawGhosts();
}

void GLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(ortho[0], ortho[1], ortho[2], ortho[3], -10, 10);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void GLWidget::UpdateSimulationSlot()
{    
    //Update ghost dynamics (it has be done first to use current pacman position)
    for(int i = 0;i < nGhosts; i++)
    {
	if(allowToPlay)
	  ghostsArray[i]->UpdateGhostPosition(pacmanArray[0]->currentPosition, pacmanArray[0]->orientation, ghostsArray[0]->currentPosition);
	ghostsCoord->replace(i, ghostsArray[i]->currentPosition);
    }
    emit UpdateGhostsPos(utilities.ConvertImageCoordToLayoutCoord(ghostsCoord, _blockWidth, _blockHeight));
  
    //Update pacman dynamics
    for(int i = 0; i < nPacman; i++)
    {
	UpdatePacmanPosition(i);
	pacmanCoord->replace(i, pacmanArray[i]->currentPosition);
    }
    emit UpdatePacmanPos(utilities.ConvertImageCoordToLayoutCoord(pacmanCoord, _blockWidth, _blockHeight));
        
    //Update cookies
    for(int i = 0;i < cookiesCoord->size();i++)
	for(int j = 0;j < nPacman;j++)
	    if (pacmanCoord->at(j) == cookiesCoord->at(i))
		cookiesCoord->remove(i);

    emit UpdateCookiesPos(utilities.ConvertImageCoordToLayoutCoord(cookiesCoord, _blockWidth, _blockHeight));
    
    //Update Bonuses
    bool enterFrigthenedMode = false;
    for(int i = 0;i < bonusCoord->size();i++)
    {
	for(int j = 0;j < nPacman;j++)
	{
	    if (pacmanCoord->at(j) == bonusCoord->at(i))
	    {
		enterFrigthenedMode |= true;
		bonusCoord->remove(i);
		
	    }
	}      
    }
    
    if (enterFrigthenedMode && !isInFrightenedMode)
    {
	isInFrightenedMode = true;
	ghostRemainingTime = ghostModeTimer->remainingTime();
	
	if (ghostRemainingTime > 0)
	    ghostModeTimer->stop();
	
	frightenedGhostModeTimer->start(frightenedModeTimeMs);
	
	for(int i = 0;i < nGhosts;i++)
	    ghostsArray[i]->SetFrigthenedMode();
    }
    else if (enterFrigthenedMode && isInFrightenedMode)
	frightenedGhostModeTimer->start(frightenedModeTimeMs);
    
    emit UpdateBonusPos(utilities.ConvertImageCoordToLayoutCoord(bonusCoord, _blockWidth, _blockHeight));
    
    //Schedule paintGL()
    update();
}

void GLWidget::ReceiveMapDataGL(int blockWidth, int blockHeight, QImage* mapImage, bool *mObstacles, QVector<int> *pPacman, QVector<int> *pGhosts, QVector<int> *pCookies, QVector<int> *pBonus)
{
    if(!firstTime)
      LoadNewTexture(mapImage);
    else
      firstTime = false;

    //Set Map
    _mapImage = new QImage(*mapImage);
    _mapWidth = mapImage->width();
    _mapHeight = mapImage->height();
    _blockWidth = blockWidth;
    _blockHeight = blockHeight;
    _obstacles = new bool[(_mapHeight)*(_mapWidth)];
    memcpy(_obstacles, mObstacles, (_mapHeight)*(_mapWidth)*sizeof(bool));
    ortho[0] = -_mapWidth*0.5;
    ortho[1] = _mapWidth*0.5;
    ortho[2] = -_mapHeight*0.5;
    ortho[3] = _mapHeight*0.5;
        
    //Set Pacman
    nPacman = pPacman->size()/2;
    pacmanArray = (Pacman**) malloc(sizeof(Pacman)*nPacman);
    pacmanCoord = new QVector<QPoint>;
    QPoint pacmanPositions;
    for(int i = 0; i < nPacman; i++)
    {
	pacmanPositions.setX(pPacman->at(i*2 + 1)*blockWidth+ortho[0]+blockWidth*0.5);
	pacmanPositions.setY(ortho[3]-(pPacman->at(i*2))*blockHeight-blockHeight*0.5);
	pacmanArray[i] = new Pacman(pacmanPositions, (double)0, blockHeight, blockWidth);
	pacmanCoord->append(pacmanArray[i]->currentPosition);
    }
        
    //Set Ghosts
    nGhosts = pGhosts->size()/2;
    ghostsArray = (Ghosts**) malloc(sizeof(Ghosts)*nGhosts);
    ghostsCoord = new QVector<QPoint>;
    QPoint ghostPositions;
    Ghosts::Personality ghostsPersonality[4] = {Ghosts::Personality::Shadow, Ghosts::Personality::Speedy, Ghosts::Personality::Bashful, Ghosts::Personality::Pokey};
    for(int i = 0; i < nGhosts; i++)
    {
	ghostPositions.setX(pGhosts->at(i*2 + 1)*blockWidth+ortho[0]+blockWidth*0.5);
	ghostPositions.setY(ortho[3]-(pGhosts->at(i*2))*blockHeight-blockHeight*0.5);
	ghostsArray[i] = new Ghosts(ghostPositions, ghostsPersonality[i], blockHeight, blockWidth, pacmanArray[0]->currentPosition, _mapHeight, _mapWidth, _obstacles);
	ghostsCoord->append(ghostsArray[i]->currentPosition);
    }
    
    //Set cookies
    cookiesCoord = new QVector<QPoint>;
    for(int i = 0; i < pCookies->size()/2; i++)
	cookiesCoord->append( *utilities.GetCoordFromIndex(_blockWidth, _blockHeight, ortho, pCookies->at(i*2), pCookies->at(i*2 + 1)) );
    
    //Set bonuses
    bonusCoord = new QVector<QPoint>;
    for(int i = 0; i < pBonus->size()/2; i++)
	bonusCoord->append( *utilities.GetCoordFromIndex(_blockWidth, _blockHeight, ortho, pBonus->at(i*2), pBonus->at(i*2 + 1)) );
    
    update();
}

void GLWidget::SetPacmanCommand(int aPacmanCommand)
{
    //TODO :modify pacman message to be able to receive up to two pacman actions
    pacmanArray[0]->action = static_cast<Pacman::Action>(aPacmanCommand);
}

void GLWidget::ToggleGhostModeSlot()
{
    for(int i = 0;i < nGhosts;i++)
	ghostsArray[i]->ToggleMode();
    
    if (contGhostModePhases < 8)
    {
	ghostModeTimer->start(ghostModeTimes[contGhostModePhases]);
	contGhostModePhases++;
    }
    isInFrightenedMode = false;
}

void GLWidget::EndOfFrightenedGhostModeSlot()
{
    for(int i = 0;i < nGhosts;i++)
	ghostsArray[i]->RecoverFromFrigthenedMode();
    
    if ((contGhostModePhases < 8) && (ghostRemainingTime > 0))
	ghostModeTimer->start(ghostRemainingTime);
    
    isInFrightenedMode = false;
}