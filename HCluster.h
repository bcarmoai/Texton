// HCluster.h: interface for the HCluster class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HCLUSTER_H__000B054C_F0E9_4E4A_9BA2_83E33E177E68__INCLUDED_)
#define AFX_HCLUSTER_H__000B054C_F0E9_4E4A_9BA2_83E33E177E68__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class HCluster  
{

public:
	void SetValuesFromFileData(HCluster* child1, HCluster* child2, HCluster* parent, int level);
	void			SetLevel(int level);
	HCluster* GetFirstChild();
	HCluster* GetSecondChild();
	HCluster* GetParent();
	void			SetParent(HCluster*);

	int				GetLevel();
	void			*GetCluster();

	bool			IsLeaf();
	bool			IsRoot();
	void			DeleteAllChildClusters();


	
	HCluster(void* aCluster, int level = 0, HCluster *hc1 = NULL, HCluster *hc2 = NULL, HCluster *parent = NULL);
	HCluster();
	virtual ~HCluster();
	
	
private:


	int			thisLevel;
	void*		thisCluster;

	HCluster*		parentCluster;
	HCluster*		childCluster1;
	HCluster*		childCluster2;

};

#endif // !defined(AFX_HCLUSTER_H__000B054C_F0E9_4E4A_9BA2_83E33E177E68__INCLUDED_)
