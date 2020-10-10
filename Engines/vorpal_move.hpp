namespace vorpal_move
{
    class Move
    {
    public:
        int from_square;
        int to_square;
        int piece;
        bool color;
        int cPiece;
        int cColor;
        bool iscapture;

        Move(int f = 0, int t = 0, int p = 1, bool c = 0, int cp = 12)
        {
            from_square = f;
            to_square = t;
            piece = p;
            color = c;
            cPiece = cp;
            cColor = !c;
            if (cp == 12)
            {
                iscapture = false;
            }
            else
            {
                iscapture = true;
            }
        }
    };
}; // namespace vorpal_move