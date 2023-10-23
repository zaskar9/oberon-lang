MODULE AVLTree;
IMPORT Out;

TYPE
  TreeNode = POINTER TO Node;
  Node = RECORD
    key, height: INTEGER;
    left, right: TreeNode
  END;

VAR
  root: TreeNode;

PROCEDURE NewTreeNode(key: INTEGER): TreeNode;
  VAR
    newNode: TreeNode;
  BEGIN
    NEW(newNode);
    newNode.key := key;
    newNode.height := 1; (* Initialize height to 1 for a new node *)
    newNode.left := NIL;
    newNode.right := NIL;
    RETURN newNode
  END NewTreeNode;

PROCEDURE Height(node: TreeNode): INTEGER;
  BEGIN
    IF node = NIL THEN
      RETURN 0
    ELSE
      RETURN node.height
    END
  END Height;

PROCEDURE MAX(a, b: INTEGER): INTEGER;
  BEGIN
    IF a > b THEN
      RETURN a
    ELSE
      RETURN b
    END
  END MAX;

PROCEDURE UpdateHeight(node: TreeNode);
  BEGIN
    IF node # NIL THEN
      node.height := 1 + MAX(Height(node.left), Height(node.right))
    END
  END UpdateHeight;

PROCEDURE RotateRight(node: TreeNode): TreeNode;
  VAR
    newRoot: TreeNode;
  BEGIN
    newRoot := node.left;
    node.left := newRoot.right;
    newRoot.right := node;

    UpdateHeight(node);
    UpdateHeight(newRoot);

    RETURN newRoot
  END RotateRight;

PROCEDURE RotateLeft(node: TreeNode): TreeNode;
  VAR
    newRoot: TreeNode;
  BEGIN
    newRoot := node.right;
    node.right := newRoot.left;
    newRoot.left := node;

    UpdateHeight(node);
    UpdateHeight(newRoot);

    RETURN newRoot
  END RotateLeft;

PROCEDURE BalanceFactor(node: TreeNode): INTEGER;
  BEGIN
    IF node = NIL THEN
      RETURN 0
    ELSE
      RETURN Height(node.left) - Height(node.right)
    END
  END BalanceFactor;

PROCEDURE Insert(VAR node: TreeNode; key: INTEGER);
  BEGIN
    IF node = NIL THEN
      node := NewTreeNode(key)
    ELSIF key < node.key THEN
      Insert(node.left, key)
    ELSE
      Insert(node.right, key)
    END;

    UpdateHeight(node);

    (* Balance the tree after insertion *)
    IF BalanceFactor(node) > 1 THEN
      IF key < node.left.key THEN
        (* Left-Left case, perform right rotation *)
        node := RotateRight(node)
      ELSE
        (* Left-Right case, perform left and then right rotation *)
        node.left := RotateLeft(node.left);
        node := RotateRight(node)
      END
    ELSIF BalanceFactor(node) < -1 THEN
      IF key > node.right.key THEN
        (* Right-Right case, perform left rotation *)
        node := RotateLeft(node)
      ELSE
        (* Right-Left case, perform right and then left rotation *)
        node.right := RotateRight(node.right);
        node := RotateLeft(node)
      END
    END
  END Insert;

(* You can also implement a Delete procedure following AVL tree deletion rules *)

PROCEDURE InOrderTraversal(node: TreeNode);
  BEGIN
    IF node # NIL THEN
      InOrderTraversal(node.left);
      Out.Int(node.key); Out.Ln; (* Print the value *)
      InOrderTraversal(node.right)
    END
  END InOrderTraversal;

BEGIN
  root := NIL; (* Initialize an empty tree *)
  (* Insert values into the AVL tree *)
  Insert(root, 10);
  Insert(root, 5);
  Insert(root, 15);
  Insert(root, 3);
  Insert(root, 7);

  (* Perform an in-order traversal to print the values in ascending order *)
  InOrderTraversal(root); Out.Ln
END AVLTree.
