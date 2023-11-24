import '../../../styles/sidebar/play/playNow.scss'
const Square = ({ isBlack, children }) => {
    const squareColor = isBlack ? 'black' : 'white';
    return <div className={`square ${squareColor}`}>{children}</div>;
};
const playNow = () => {
    const board = Array(8).fill(null).map((row, rowIndex) => {
        return Array(8).fill(null).map((square, colIndex) => {
            const isBlack = (rowIndex + colIndex) % 2 === 1;
            return <Square key={`${rowIndex}-${colIndex}`} isBlack={isBlack} />;
        });
    });

    return <div className="chessboard">{board}</div>;
}

export default playNow;