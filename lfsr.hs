import Data.Bits
import Data.Word
import Control.Arrow

modify :: (Bits a) => Int -> (a -> a)
                      -> a -> a
modify stride update = uncurry (.|.) . (flip shift stride &&& update)

lfsr :: (Bits a, Num a) => a -> a -> a
lfsr taps = modify 1 doLFSR
          where doLFSR bits = fromIntegral $ (1 + popCount bits) `mod` 2

takePeriod :: (Eq a) => (a -> a) -> a -> [a]
takePeriod f init = init : (takeWhile (/= init) . tail $ iterate f init)

getLFSRPeriod (taps, init) = ((taps, init), period)
                           where period = takePeriod (lfsr (taps :: Word16)) init

tapRange  = [0..65535]
initRange = [0..65535]
main = mapM (putStrLn . show . getLFSRPeriod) [(x,y) | x <- tapRange, y <- initRange]
