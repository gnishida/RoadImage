/**
 * 道路網を、画像として保存する。
 *
 * @author Gen Nishida
 * @version 1.0
 */

#include <iostream>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "GraphUtil.h"

int main(int argc, char *argv[]) {
	if (argc < 4) {
		std::cerr << "Usage: RoadImage <road GSM file> <road type> <cell size>" << std::endl;
		return 1;
	}
	
	QString filename = argv[1];
	int roadType = atoi(argv[2]);
	int cellSize = atoi(argv[3]);

	// ファイル名のベースパートを取得
	QString basename = filename.split("\\").last().split(".").at(0);

	// 道路網をGSMファイルから読み込む
	RoadGraph r;
	GraphUtil::loadRoads(r, filename, roadType);

	int margin = 50;

	BBox box = GraphUtil::getAABoundingBox(r);

	// 指定されたセルサイズに分割する
	int count = 0;
	for (int y = box.minPt.y(); y <= box.maxPt.y() - cellSize * 0.5; y += cellSize) {
		for (int x = box.minPt.x(); x <= box.maxPt.x() - cellSize * 0.5; x += cellSize) {
			BBox area(QVector2D(x, y));
			area.addPoint(QVector2D(x + cellSize, y + cellSize));

			// パッチの範囲の道路網を抽出
			RoadGraph patch;
			GraphUtil::copyRoads(r, patch);
			GraphUtil::extractRoads2(patch, area);

			// 道路網が、直交座標系の第一象限に位置するよう、移動する
			QVector2D offset;
			offset.setX(-area.minPt.x() + margin);
			offset.setY(-area.minPt.y() + margin);
			GraphUtil::translate(patch, offset);

			// cv::Matを作成
			cv::Mat_<uchar> mat(area.dy() + margin * 2, area.dx() + margin * 2);
			GraphUtil::convertToMat(patch, mat, mat.size());

			// 画像として保存
			cv::imwrite((basename + "%1.jpg").arg(count).toUtf8().data(), mat);

			count++;
		}
	}
}