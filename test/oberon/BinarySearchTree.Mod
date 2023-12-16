MODULE BinarySearchTree;
IMPORT Out;

TYPE
  TreeNode = POINTER TO Node;
  Node = RECORD
    key: INTEGER;
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
    newNode.left := NIL;
    newNode.right := NIL;
    RETURN newNode
  END NewTreeNode;

PROCEDURE MinValueNode(node: TreeNode): TreeNode;
  BEGIN
    WHILE node.left # NIL DO
      node := node.left
    END;
    RETURN node
  END MinValueNode;

PROCEDURE Insert(VAR node: TreeNode; key: INTEGER);
  BEGIN
    IF node = NIL THEN
      node := NewTreeNode(key)
    ELSIF key < node.key THEN
      Insert(node.left, key)
    ELSE
      Insert(node.right, key)
    END
  END Insert;

PROCEDURE Delete(VAR node: TreeNode; key: INTEGER);
  VAR
    tempNode: TreeNode;
  BEGIN
    IF node = NIL THEN
      RETURN (* Key not found in the tree *)
    ELSIF key < node.key THEN
      Delete(node.left, key)
    ELSIF key > node.key THEN
      Delete(node.right, key)
    ELSE (* Key is found *)
      IF node.left = NIL THEN
        tempNode := node.right;
        (* FREE(node); <- coming soon to oberon-lang *)
        node := tempNode
      ELSIF node.right = NIL THEN
        tempNode := node.left;
        (* FREE(node); <- coming soon to oberon-lang *)
        node := tempNode
      ELSE
        tempNode := MinValueNode(node.right);
        node.key := tempNode.key;
        Delete(node.right, tempNode.key)
      END
    END
  END Delete;

PROCEDURE InOrderTraversal(node: TreeNode);
  BEGIN
    IF node # NIL THEN
      InOrderTraversal(node.left);
      (* Print the value *)
      Out.Int(node.key); Out.String(" ");
      InOrderTraversal(node.right)
    END
  END InOrderTraversal;

BEGIN
  (* Initialize an empty tree *)
  root := NIL;
  (* Insert values into the binary tree *)
  Insert(root, 10);
  Insert(root, 5);
  Insert(root, 15);
  Insert(root, 3);
  Insert(root, 7);

  (* Perform an in-order traversal to print the values in ascending order *)
  InOrderTraversal(root);
  Out.Ln;

  (* Delete a node with a given key, for example, delete the node with key 5 *)
  Delete(root, 5);

  (* Perform an in-order traversal after deletion to see the updated tree *)
  InOrderTraversal(root);
  Out.Ln
END BinarySearchTree.
