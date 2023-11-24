// import io from 'socket.io-client';

const initState = {
    user: [
        { id: 1, username: "namidol", password: "1234", name: "nam", age: 18, gender: "female" },
        { id: 2, username: "nhanidol", password: "4567", name: "nhan", age: 18, gender: "male" }
    ]
}

const Signup = (state = initState, action) => {
    switch (action.type) {
        case 'SIGNUP':
            return {
                ...state, Login: [...state.user, action.payload]
            }
        default:
            return state;
    }
}

export default Signup;