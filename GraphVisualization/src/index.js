'use strict';
(() => {

    const s = new sigma({
        renderer: {
            container: document.getElementById('sigma-container'),
            type: 'canvas'
        },
        settings: {
            minEdgeSize: 0.1,
            maxEdgeSize: 1,
            minNodeSize: 1,
            maxNodeSize: 8,
        }
    });

    const NODE_AMOUNT = 20;

    const getRoots = (nodes, edges) => {
        const descendentsSet = new Set(R.pluck("target", edges));
        return nodes.filter(n => !descendentsSet.has(n.id));
    };

    const setMaxDepth = (roots, nodes, adjacencyMap, depth = 0) => {
        for (const r of roots) {
            r.depth = r.depth || 0;
            r.depth = depth > r.depth ? depth : r.depth;
            const edges = adjacencyMap[r.id];
            if (!edges) continue;
            const children = edges.map(({target}) => nodes[target]);
            setMaxDepth(children, nodes, adjacencyMap, depth + 1);
        }
    };

    const pickRandomNodeId = (max, min = 0) => min + Math.floor(Math.random() * (max - min));

    const bigNodes = R.times(id => ({
        id,
        label: `node ${id}`,
        x: Math.random(),
        y: Math.random(),
        size: 1,
        color: '#FF0000'
    }), NODE_AMOUNT);

    const bigEdges = R.times(id => {
        const sourceId = pickRandomNodeId(bigNodes.length - 1);
        const targetId = pickRandomNodeId(bigNodes.length, sourceId + 1);
        return ({
            id,
            source: bigNodes[sourceId].id,
            target: bigNodes[targetId].id,
            type: 'curvedArrow',
            color: '#8888FF',
            size: 2,
        });
    }, NODE_AMOUNT * 4);

    const nodeLookup = R.pipe(R.map(n => ({[n.id]: n})), R.mergeAll)(bigNodes);

    const adjacencyMap = R.groupBy(R.prop("source"), bigEdges);

    const roots = getRoots(bigNodes, bigEdges);
    setMaxDepth(roots, nodeLookup, adjacencyMap);

    const maxDepth = R.pipe(R.pluck("depth"), R.reduce(R.max, 0))(bigNodes);
    const levels = R.groupBy(R.prop("depth"), bigNodes);

    for (const n of bigNodes){
        n.y = 1.0 / (levels[n.depth].findIndex(R.propEq("id", n.id)) + 1);
        n.x = n.depth / maxDepth;
    }


    console.log({maxDepth});
    console.log(roots);
    console.log(nodeLookup);
    console.log(adjacencyMap);

    const bigGraph = {
        nodes: bigNodes,
        edges: bigEdges,
    };

    s.graph.read(bigGraph);

    s.refresh();
})();
