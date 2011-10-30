#include "BinnedParticleSystem.h"

BinnedParticleSystem::BinnedParticleSystem() :
	timeStep(100) {
}

void BinnedParticleSystem::setup(int width, int height, int k) {
	this->width = width;
	this->height = height;
	this->k = k;
	binSize = 1 << k;
	xBins = (int) ceilf((float) width / (float) binSize);
	yBins = (int) ceilf((float) height / (float) binSize);
	bins.resize(xBins * yBins);
}

void BinnedParticleSystem::setTimeStep(float timeStep) {
	this->timeStep = timeStep;
}

void BinnedParticleSystem::add(BinnedParticle particle) {
	particles.push_back(particle);
}

unsigned BinnedParticleSystem::size() const {
	return particles.size();
}

BinnedParticle& BinnedParticleSystem::operator[](unsigned i) {
	return particles[i];
}

vector<BinnedParticle*> BinnedParticleSystem::getNeighbors(BinnedParticle& particle, float radius) {
	return getNeighbors(particle.x, particle.y, radius);
}

vector<BinnedParticle*> BinnedParticleSystem::getNeighbors(float x, float y, float radius) {
	vector<BinnedParticle*> region = getRegion(
		(int) (x - radius),
		(int) (y - radius),
		(int) (x + radius),
		(int) (y + radius));
	vector<BinnedParticle*> neighbors;
	int n = region.size();
	float xd, yd, rsq, maxrsq;
	maxrsq = radius * radius;
	for(int i = 0; i < n; i++) {
		BinnedParticle& cur = *region[i];
		xd = cur.x - x;
		yd = cur.y - y;
		rsq = xd * xd + yd * yd;
		if(rsq < maxrsq)
			neighbors.push_back(region[i]);
	}
	return neighbors;
}

vector<BinnedParticle*> BinnedParticleSystem::getRegion(unsigned minX, unsigned minY, unsigned maxX, unsigned maxY) {
	vector<BinnedParticle*> region;
	back_insert_iterator< vector<BinnedParticle*> > back = back_inserter(region);
	unsigned minXBin = minX >> k;
	unsigned maxXBin = maxX >> k;
	unsigned minYBin = minY >> k;
	unsigned maxYBin = maxY >> k;
	maxXBin++;
	maxYBin++;
	if(maxXBin > xBins)
		maxXBin = xBins;
	if(maxYBin > yBins)
		maxYBin = yBins;
	for(int y = minYBin; y < maxYBin; y++) {
		for(int x = minXBin; x < maxXBin; x++) {
			vector<BinnedParticle*>& cur = bins[y * xBins + x];
			copy(cur.begin(), cur.end(), back);
		}
	}
	return region;
}

void BinnedParticleSystem::setupForces() {
	int n = bins.size();
	for(int i = 0; i < n; i++) {
		bins[i].clear();
	}
	n = particles.size();
	unsigned xBin, yBin, bin;
	for(int i = 0; i < n; i++) {
		BinnedParticle& cur = particles[i];
		cur.resetForce();
		xBin = ((unsigned) cur.x) >> k;
		yBin = ((unsigned) cur.y) >> k;
		bin = yBin * xBins + xBin;
		if(xBin < xBins && yBin < yBins)
			bins[bin].push_back(&cur);
	}
}

void BinnedParticleSystem::addRepulsionForce(const BinnedParticle& particle, float radius, float scale) {
	addRepulsionForce(particle.x, particle.y, radius, scale);
}

void BinnedParticleSystem::addRepulsionForce(float x, float y, float radius, float scale) {
	addForce(x, y, radius, scale);
}

void BinnedParticleSystem::addAttractionForce(const BinnedParticle& particle, float radius, float scale) {
	addAttractionForce(particle.x, particle.y, radius, scale);
}

void BinnedParticleSystem::addAttractionForce(float x, float y, float radius, float scale) {
	addForce(x, y, radius, -scale);
}

void BinnedParticleSystem::addForce(const BinnedParticle& particle, float radius, float scale) {
	addForce(particle.x, particle.y, radius, -scale);
}

void BinnedParticleSystem::addForce(float targetX, float targetY, float radius, float scale) {
	float minX = targetX - radius;
	float minY = targetY - radius;
	float maxX = targetX + radius;
	float maxY = targetY + radius;
	if(minX < 0)
		minX = 0;
	if(minY < 0)
		minY = 0;
	unsigned minXBin = ((unsigned) minX) >> k;
	unsigned minYBin = ((unsigned) minY) >> k;
	unsigned maxXBin = ((unsigned) maxX) >> k;
	unsigned maxYBin = ((unsigned) maxY) >> k;
	maxXBin++;
	maxYBin++;
	if(maxXBin > xBins)
		maxXBin = xBins;
	if(maxYBin > yBins)
		maxYBin = yBins;
	float xd, yd, length, maxrsq;
	#ifdef USE_INVSQRT
	float xhalf;
	int lengthi;
	#else
	float effect;
	#endif
	maxrsq = radius * radius;
	for(int y = minYBin; y < maxYBin; y++) {
		for(int x = minXBin; x < maxXBin; x++) {
			vector<BinnedParticle*>& curBin = bins[y * xBins + x];
			int n = curBin.size();
			for(int i = 0; i < n; i++) {
				BinnedParticle& curBinnedParticle = *(curBin[i]);
				xd = curBinnedParticle.x - targetX;
				yd = curBinnedParticle.y - targetY;
				length = xd * xd + yd * yd;
				if(length > 0 && length < maxrsq) {
					#ifdef DRAW_FORCES
						glVertex2f(targetX, targetY);
						glVertex2f(curBinnedParticle.x, curBinnedParticle.y);
					#endif
					#ifdef USE_INVSQRT
						xhalf = 0.5f * length;
						lengthi = *(int*) &length;
						lengthi = 0x5f3759df - (lengthi >> 1);
						length = *(float*) &lengthi;
						length *= 1.5f - xhalf * length * length;
						xd *= length;
						yd *= length;
						length *= radius;
						length = 1 / length;
						length = (1 - length);
						#ifdef USE_SMOOTH_FORCES
							length = smoothForce(length);
						#endif
						length *= scale;
						xd *= length;
						yd *= length;
						curBinnedParticle.xf += xd;
						curBinnedParticle.yf += yd;
					#else
						length = sqrtf(length);
						#ifdef USE_SMOOTH_FORCES
							length = smoothForce(length);
						#endif
						xd /= length;
						yd /= length;
						effect = (1 - (length / radius)) * scale;
						curBinnedParticle.xf += xd * effect;
						curBinnedParticle.yf += yd * effect;
					#endif
				}
			}
		}
	}
}

void BinnedParticleSystem::update(float lastTimeStep) {
	int n = particles.size();
	float curTimeStep = lastTimeStep * timeStep;
	for(int i = 0; i < n; i++) {
		particles[i].updatePosition(curTimeStep);
	}
}

void BinnedParticleSystem::draw() {
	int n = particles.size();
	glBegin(GL_POINTS);
	for(int i = 0; i < n; i++)
		particles[i].draw();
	glEnd();
}

int BinnedParticleSystem::getWidth() const {
	return width;
}

int BinnedParticleSystem::getHeight() const {
	return height;
}
